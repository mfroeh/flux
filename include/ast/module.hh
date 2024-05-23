#pragma once

#include "ast.hh"
#include "ast/class.hh"
#include "function.hh"
#include "visitor.hh"

using std::shared_ptr;
using std::vector;

struct Module : public Node {
  std::filesystem::path path;
  vector<std::filesystem::path> includes;
  vector<FunctionDefinition> functions;
  vector<ClassDefinition> classes;

  Module(Tokens tokens, std::filesystem::path file,
         vector<std::filesystem::path> includes,
         vector<ClassDefinition> classes, vector<FunctionDefinition> functions);

  any accept(AbstractAstVisitor &visitor) override;
};