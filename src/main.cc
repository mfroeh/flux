#include "ANTLRFileStream.h"
#include "ANTLRInputStream.h"
#include "FluxLexer.h"
#include "FluxParser.h"
#include "analysis/desugar.hh"
#include "analysis/typecheck.hh"
#include "analysis/variable_resolver.hh"
#include "ast/ast_creator.hh"
#include "codegen/ir_visitor.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include "visitor.hh"
#include <argparse/argparse.hpp>
#include <cassert>
#include <filesystem>
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
#include <system_error>

using namespace std;
using std::filesystem::path;

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
  assert(files.size() == 1);

  for (auto &file : files) {
    auto path = filesystem::path(file);
    if (!filesystem::exists(path) || !filesystem::is_regular_file(path)) {
      cerr << "error: file not found: " << file << endl;
      return 1;
    }

    auto stream = ifstream(path);
    if (!stream) {
      cerr << "error: failed to open file: " << file << endl;
      return 1;
    }

    auto moduleContext = ModuleContext(path);

    cout << "Lexing " << file << endl;
    antlr4::ANTLRInputStream input(stream);
    FluxLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    for (auto &token : tokens.getTokens()) {
      cout << token->toString() << endl;
    }

    cout << "Parsing " << file << endl;
    FluxParser parser(&tokens);
    parser.setBuildParseTree(true);
    auto parseTreeRoot = parser.module();

    cout << "Creating AST " << file << endl;
    auto astCreator = make_shared<AstCreator>();
    Module module = astCreator->visitModule(parseTreeRoot);

    SymbolTable symTab;

    cout << "Resolving variable references " << file << endl;
    auto resolver = make_shared<VariableResolver>(moduleContext, symTab);
    static_pointer_cast<AstVisitor>(resolver)->visit(module);

    cout << "Type checking and resolving function calls" << file << endl;
    auto typeChecker = make_shared<TypeChecker>(moduleContext, symTab);
    static_pointer_cast<AstVisitor>(typeChecker)->visit(module);

    cout << "Desugaring " << file << endl;
    auto desugarer = make_shared<Desugarer>(moduleContext);
    desugarer->visit(module);

    cout << "Generating IR code" << file << endl;
    auto codegenContext = make_shared<CodegenContext>();
    auto irVisitor =
        make_shared<IRVisitor>(moduleContext, symTab, *codegenContext);
    auto llvmModule = irVisitor->visit(module);

    llvm::outs() << *llvmModule;

    cout << "Optimizing IR code" << file << endl;

    cout << "Creating object file" << file << endl;
    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::CGFT_ObjectFile;

    auto mainFunctions = symTab.getFunctions("main");
    if (mainFunctions.size() != 1) {
      cerr << "Expected exactly one main function" << endl;
      return 1;
    }
    auto mainFunction = mainFunctions.front();
    // rename main function to "main" (this should be done whilst compiling if
    // main is called recursively)
    mainFunction->llvmFunction->setName("main");

    error_code ec;
    auto outStream = llvm::raw_fd_ostream("a.out", ec, llvm::sys::fs::OF_None);
    if (codegenContext->targetMachine->addPassesToEmitFile(pass, outStream,
                                                           nullptr, fileType)) {
      cerr << "TargetMachine can't emit a file of this type" << endl;
      return 1;
    }
    pass.run(*llvmModule);
    outStream.flush();

    // string err;
    // auto ee =
    //     llvm::EngineBuilder(std::move(llvmModule)).setErrorStr(&err).create();

    // if (!ee) {
    //   cerr << "Failed to create ExecutionEngine: " << err << endl;
    //   return 1;
    // }

    // auto main = symTab.getFunctions("main").front();
    // if (!main) {
    //   cerr << "No main function found" << endl;
    //   return 1;
    // }

    // cout << "Executing main" << endl;
    // auto result = ee->runFunction(main->llvmFunction, {});
    // cout << "Result: " << result.IntVal.getSExtValue() << endl;
  }
  return 0;
}