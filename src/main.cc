#include "ANTLRFileStream.h"
#include "ANTLRInputStream.h"
#include "FluxLexer.h"
#include "FluxParser.h"
#include "analysis/desugar.hh"
#include "analysis/typecheck.hh"
#include "analysis/variable_resolver.hh"
#include "ast/ast_creator.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include "visitor.hh"
#include <argparse/argparse.hpp>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

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

    cout << "Desugaring " << file << endl;
    auto desugarer = make_shared<Desugarer>(moduleContext);
    desugarer->visit(module);

    cout << "Resolving variable references " << file << endl;
    auto resolver = make_shared<VariableResolver>(moduleContext, symTab);
    static_pointer_cast<AstVisitor>(resolver)->visit(module);

    cout << "Type checking and resolving function calls" << file << endl;
    auto typeChecker = make_shared<TypeChecker>(moduleContext, symTab);
    static_pointer_cast<AstVisitor>(typeChecker)->visit(module);
  }
  return 0;
}