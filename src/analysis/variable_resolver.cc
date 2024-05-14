#include "analysis/variable_resolver.hh"
#include "ast/expr.hh"
#include "visitor.hh"

using namespace std;

VariableResolver::VariableResolver(ModuleContext &context)
    : AstVisitor(context) {
  currentScope = make_shared<Scope>();
}

any VariableResolver::visit(FunctionDefinition &functionDefinition) {
  auto previousScope = currentScope;
  currentScope = make_shared<Scope>(previousScope);

  AstVisitor::visit(functionDefinition);

  currentScope = previousScope;
  return {};
}

any VariableResolver::visit(IfElse &ifElse) {
  auto previousScope = currentScope;
  currentScope = make_shared<Scope>(previousScope);

  AstVisitor::visit(ifElse);

  currentScope = previousScope;
  return {};
}

any VariableResolver::visit(While &whileLoop) {
  auto previousScope = currentScope;
  currentScope = make_shared<Scope>(previousScope);

  AstVisitor::visit(whileLoop);

  currentScope = previousScope;
  return {};
}

any VariableResolver::visit(Block &block) {
  auto previousScope = currentScope;
  if (block.isStandalone)
    currentScope = make_shared<Scope>(previousScope);

  AstVisitor::visit(block);

  if (block.isStandalone)
    currentScope = previousScope;
  return {};
}

any VariableResolver::visit(Parameter &parameter) {
  currentScope->addVariable(parameter.name, parameter.type);
  return {};
}

any VariableResolver::visit(VariableDeclaration &variableDeclaration) {
  if (variableDeclaration.initializer)
    variableDeclaration.initializer->accept(*this);

  currentScope->addVariable(variableDeclaration.name, variableDeclaration.type);
  return {};
}

any VariableResolver::visit(VariableReference &var) {
  auto variable = currentScope->getVariable(var.name);
  if (!variable)
    throw runtime_error("Variable " + var.name + " not found");

  var.name = variable->mangledName;
  return {};
}

any VariableResolver::visit(ArrayReference &arr) {
  arr.index->accept(*this);
  auto variableRef = dynamic_pointer_cast<VariableReference>(arr.arrayExpr);
  if (variableRef == nullptr)
    return {};

  string name = variableRef->name;
  auto variable = currentScope->getVariable(name);
  if (!variable)
    throw runtime_error("Variable " + name + " not found");

  variableRef->name = variable->mangledName;
  return {};
}