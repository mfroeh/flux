#pragma once

#include "ast/ast.hh"
#include "ast/stmt.hh"
#include "ast/type.hh"
#include <memory>
#include <ostream>

using std::shared_ptr;
using std::string;
using std::vector;

struct Parameter : public Node {
  string name;
  shared_ptr<Type> type;

  // set during name resolution
  string mangledName;

  Parameter(Tokens tokens, string name, shared_ptr<Type> type);

  any accept(AbstractAstVisitor &visitor) override;
};

struct FunctionDefinition : public Statement {
  string name;
  vector<Parameter> parameters;
  shared_ptr<Type> returnType;
  Block body;

  // set during name resolution
  string mangledName;

  // only set if function is method
  shared_ptr<ClassType> classType;

  FunctionDefinition(Tokens tokens, string name, vector<Parameter> parameters,
                     shared_ptr<Type> returnType, Block body);

  any accept(AbstractAstVisitor &visitor) override;

  void codegen(struct IRVisitor &visitor) override;
};