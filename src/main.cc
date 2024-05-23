#include "ANTLRFileStream.h"
#include "ANTLRInputStream.h"
#include "FluxLexer.h"
#include "FluxParser.h"
#include "analysis/desugar.hh"
#include "analysis/typecheck.hh"
#include "analysis/variable_resolver.hh"
#include "ast/ast_creator.hh"
#include "ast/type.hh"
#include "codegen/ir_visitor.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include "visitor.hh"
#include <argparse/argparse.hpp>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <system_error>
#include <unordered_map>

using namespace std;
using std::filesystem::path;

void exitWithError(const path &file, const string &message) {
  throw runtime_error(std::format("[{}]: {}", file.c_str(), message));
}

// https://en.wikipedia.org/wiki/Topological_sorting
vector<shared_ptr<Module>> topologicalSort(
    const unordered_map<filesystem::path, shared_ptr<Module>> &modules) {
  vector<shared_ptr<Module>> L;
  stack<filesystem::path> S;

  for (auto &[path, module] : modules) {
    if (module->includes.empty())
      S.push(path);
  }

  while (!S.empty()) {
    auto n = S.top();
    S.pop();
    L.push_back(modules.at(n));

    // for each node m with an edge e from n to m do
    auto M = modules | ranges::views::filter([&n](auto &p) {
               return ranges::find_if(p.second->includes, [&](auto &pair) {
                        return filesystem::equivalent(pair, n);
                      }) != p.second->includes.end();
             });
    for (auto &m : M) {
      // remove the edge
      m.second->includes.erase(
          ranges::find_if(m.second->includes, [&n](auto &include) {
            return filesystem::equivalent(include, n);
          }));

      if (m.second->includes.empty())
        S.push(m.first);
    }
  }

  if (L.empty())
    throw runtime_error("graph has at least one cycle");

  return L;
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("flux");
  program.add_argument("files", "Files to compile")
      .nargs(argparse::nargs_pattern::at_least_one);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    cerr << err.what() << endl;
    cerr << program;
    return 1;
  }

  auto files = program.get<std::vector<std::string>>("files");
  if (files.size() > 1) {
    throw runtime_error(
        "Only single file compilation is supported, use `incl` instead.");
  }

  for (auto &file : files) {
    auto path = filesystem::path(file);
    if (!filesystem::exists(path) || !filesystem::is_regular_file(path))
      exitWithError(file, "no such file");
  }

  // resolve all includes
  unordered_map<filesystem::path, shared_ptr<Module>> modules;
  queue<filesystem::path> unvisited;
  unvisited.push(files.front());

  do {
    auto current = unvisited.front();
    if (!filesystem::exists(current) || !filesystem::is_regular_file(current))
      exitWithError(current, "no such file");

    auto stream = ifstream(current);
    if (!stream)
      exitWithError(current, "failed to open file");

    ModuleContext moduleContext = ModuleContext(current);
    antlr4::ANTLRInputStream input(stream);
    FluxLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    if (lexer.getNumberOfSyntaxErrors())
      exitWithError(current, "syntax error");

    FluxParser parser(&tokens);
    parser.setBuildParseTree(true);
    auto parseTreeRoot = parser.module();

    if (parser.getNumberOfSyntaxErrors())
      exitWithError(current, "syntax error");

    auto astCreator = make_shared<AstCreator>(moduleContext);
    Module module = astCreator->visitModule(parseTreeRoot);

    modules[current] = make_shared<Module>(module);
    for (auto &include : module.includes) {
      auto it = ranges::find_if(modules, [&include](auto &p) {
        return filesystem::equivalent(p.first, include);
      });

      if (it == modules.end()) {
        unvisited.push(include);
        cout << "added" << include << endl;
      }
    }
    unvisited.pop();
  } while (!unvisited.empty());

  vector<shared_ptr<Module>> sortedModules = topologicalSort(modules);
  Module module = *sortedModules.front();
  ModuleContext moduleContext = ModuleContext(module.path);
  for (int i = 1; i < sortedModules.size(); i++) {
    ranges::copy(sortedModules[i]->classes, back_inserter(module.classes));
    ranges::copy(sortedModules[i]->functions, back_inserter(module.functions));
    cout << " " << sortedModules[i]->path << "\n" << endl;
  }

  SymbolTable symTab;

  shared_ptr<Desugarer> desugarer =
      make_shared<NonTypedDesugarer>(moduleContext);
  desugarer->visit(module);

  auto resolver = make_shared<VariableResolver>(moduleContext, symTab);
  static_pointer_cast<AstVisitor>(resolver)->visit(module);

  auto typeChecker = make_shared<TypeChecker>(moduleContext, symTab);
  static_pointer_cast<AstVisitor>(typeChecker)->visit(module);

  shared_ptr<Desugarer> typedDesugarer =
      make_shared<TypedDesugarer>(moduleContext, symTab);
  typedDesugarer->visit(module);

  auto codegenContext = make_shared<CodegenContext>();
  auto irVisitor =
      make_shared<IRVisitor>(moduleContext, symTab, *codegenContext);
  auto llvmModule = irVisitor->visit(module);

  llvm::outs() << *llvmModule;

  llvm::legacy::PassManager pass;
  auto fileType = llvm::CodeGenFileType::CGFT_ObjectFile;

  auto mainFunctions = symTab.getFunctions("main");
  if (mainFunctions.size() != 1)
    exitWithError(module.path, "expected exactly one main function");

  auto mainFunction = mainFunctions.front();
  if (mainFunction->parameters.size() != 0)
    exitWithError(module.path, "main function should not have any parameters");

  if (mainFunction->returnType != IntType::get())
    exitWithError(module.path, "main function should return an int");

  // rename main function to "main" (this should be done whilst compiling if
  // main is called recursively)
  mainFunction->llvmFunction->setName("main");

  error_code ec;
  auto outStream = llvm::raw_fd_ostream("a.out", ec, llvm::sys::fs::OF_None);
  auto failed = codegenContext->targetMachine->addPassesToEmitFile(
      pass, outStream, nullptr, fileType);

  if (failed)
    exitWithError(module.path,
                  "target machine can not emit a file of this type");

  pass.run(*llvmModule);
  outStream.flush();

  return 0;
}