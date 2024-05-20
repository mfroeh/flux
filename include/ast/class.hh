#pragma once

#include "ast/ast.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"
#include "ast/type.hh"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct FieldDeclaration : public Node {
  string name;
  shared_ptr<Type> type;
  shared_ptr<ClassType> classType;

  // assigned during name resolution
  string mangledName;

  FieldDeclaration(Tokens tokens, string name, shared_ptr<Type> type);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct ClassDefinition : public Node {
  string name;
  vector<FieldDeclaration> fields;
  vector<FunctionDefinition> methods;

  shared_ptr<ClassType> type;

  ClassDefinition(Tokens tokens, string name, vector<FieldDeclaration> fields,
                  vector<FunctionDefinition> methods);

  any accept(class AbstractAstVisitor &visitor) override;
  void codegen(class IRVisitor &visitor);
};
