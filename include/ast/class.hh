#pragma once

#include "ast/ast.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct MethodDefinition : public FunctionDefinition {
  shared_ptr<struct ClassDefinition> klass;

  MethodDefinition(Tokens tokens, string name, vector<Parameter> parameters,
                   Block body, shared_ptr<Type> returnType);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct FieldDeclaration : public Node {
  string name;
  shared_ptr<Type> type;

  shared_ptr<struct ClassDefinition> klass;

  FieldDeclaration(Tokens tokens, string name, shared_ptr<Type> type);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct ClassDefinition : public Node,
                         public std::enable_shared_from_this<ClassDefinition> {
  string name;
  vector<FieldDeclaration> fields;
  vector<MethodDefinition> methods;

  ClassDefinition(Tokens tokens, string name, vector<FieldDeclaration> fields,
                  vector<MethodDefinition> methods);

  any accept(class AbstractAstVisitor &visitor) override;
};
