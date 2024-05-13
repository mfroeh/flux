#pragma once

#include "ast.hh"
#include "function.hh"
#include "visitor.hh"

using std::unique_ptr;
using std::vector;

struct Module : public Node {
  vector<unique_ptr<FunctionDefinition>> functions;

  Module(Tokens tokens, vector<unique_ptr<FunctionDefinition>> functions);

  any accept(AstVisitor &visitor) override;
};