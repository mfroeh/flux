#pragma once

#include "ast/ast.hh"
#include "ast/stmt.hh"
#include "ast/type.hh"
#include <memory>

using std::string;
using std::unique_ptr;
using std::vector;

struct Parameter : public Node {
  string name;
  unique_ptr<Type> type;

  Parameter(Tokens tokens, string name, unique_ptr<Type> type);

  any accept(AstVisitor &visitor) override;
};

struct FunctionDefinition : public Node {
  string name;
  vector<Parameter> parameters;
  unique_ptr<Type> returnType;
  unique_ptr<Block> body;

  FunctionDefinition(Tokens tokens, string name, vector<Parameter> parameters,
                     unique_ptr<Type> returnType, unique_ptr<Block> body);

  any accept(AstVisitor &visitor) override;

  llvm::Function *codegen(IRVisitor &visitor);
};