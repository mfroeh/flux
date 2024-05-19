#include "ast/class.hh"
#include "ast/function.hh"

#include "codegen/ir_visitor.hh"
#include "visitor.hh"

FieldDeclaration::FieldDeclaration(Tokens tokens, string name,
                                   shared_ptr<Type> type)
    : Node(std::move(tokens)), name(std::move(name)), type(std::move(type)) {}

any FieldDeclaration::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

ClassDefinition::ClassDefinition(Tokens tokens, string name,
                                 vector<FieldDeclaration> fields,
                                 vector<FunctionDefinition> methods)
    : Node(tokens), name(name), fields(fields), methods(std::move(methods)),
      type(ClassType::get(name)) {}

any ClassDefinition::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

void ClassDefinition::codegen(IRVisitor &visitor) { visitor.visit(*this); }