#pragma once

#include "ast.hh"
#include "ast/class.hh"
#include "function.hh"
#include "visitor.hh"

using std::shared_ptr;
using std::vector;

struct Module : public Node {
  vector<FunctionDefinition> functions;
  vector<ClassDefinition> classes;

  Module(Tokens tokens, vector<ClassDefinition> classes,
         vector<FunctionDefinition> functions);

  any accept(AbstractAstVisitor &visitor) override;
};