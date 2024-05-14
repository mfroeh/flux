#pragma once

#include "ast.hh"
#include "function.hh"
#include "visitor.hh"

using std::shared_ptr;
using std::vector;

struct Module : public Node {
  vector<FunctionDefinition> functions;

  Module(Tokens tokens, vector<FunctionDefinition> functions);

  any accept(AbstractAstVisitor &visitor) override;
};