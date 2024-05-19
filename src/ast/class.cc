#include "ast/class.hh"

#include "visitor.hh"

MethodDefinition::MethodDefinition(Tokens tokens, string name,
                                   vector<Parameter> parameters, Block body,
                                   shared_ptr<Type> returnType)
    : FunctionDefinition(tokens, name, parameters, returnType, body) {}

any MethodDefinition::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

FieldDeclaration::FieldDeclaration(Tokens tokens, string name,
                                   shared_ptr<Type> type)
    : Node(std::move(tokens)), name(std::move(name)), type(std::move(type)) {}

any FieldDeclaration::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

ClassDefinition::ClassDefinition(Tokens tokens, string name,
                                 vector<FieldDeclaration> fields,
                                 vector<MethodDefinition> methods)
    : Node(std::move(tokens)), name(std::move(name)), fields(std::move(fields)),
      methods(std::move(methods)) {
  for (auto &field : this->fields) {
    field.klass = shared_from_this();
  }

  for (auto &method : this->methods) {
    method.klass = shared_from_this();
  }
}

any ClassDefinition::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}
