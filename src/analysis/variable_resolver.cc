#include "analysis/variable_resolver.hh"
#include "ast/expr.hh"
#include "ast/stmt.hh"
#include "symbol_table.hh"
#include "visitor.hh"

using namespace std;

VariableResolver::VariableResolver(ModuleContext &context, SymbolTable &symTab)
    : AstVisitor(context, symTab) {
  currentScope = make_shared<Scope>(nullptr);
}

any VariableResolver::visit(FunctionDefinition &functionDefinition) {
  // add function to current scope
  currentScope->addFunction(functionDefinition.name,
                            functionDefinition.returnType,
                            functionDefinition.parameters);
  auto function = currentScope->getFunction(functionDefinition.name);
  symTab.insert(function);
  functionDefinition.mangledName = function->mangledName;

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

any VariableResolver::visit(StandaloneBlock &block) {
  auto previousScope = currentScope;
  currentScope = make_shared<Scope>(previousScope);

  cout << "Visiting standalone block" << endl;
  AstVisitor::visit(block);
  cout << "Leaving standalone block" << endl;

  currentScope = previousScope;
  return {};
}

any VariableResolver::visit(Parameter &parameter) {
  currentScope->addVariable(parameter.name, parameter.type);
  auto variable = currentScope->getVariable(parameter.name);
  symTab.insert(variable);
  parameter.name = variable->mangledName;
  return {};
}

any VariableResolver::visit(VariableDeclaration &variableDeclaration) {
  if (variableDeclaration.initializer)
    variableDeclaration.initializer->accept(*this);

  currentScope->addVariable(variableDeclaration.name, variableDeclaration.type);
  auto variable = currentScope->getVariable(variableDeclaration.name);
  symTab.insert(variable);
  variableDeclaration.mangledName = variable->mangledName;
  return {};
}

any VariableResolver::visit(VariableReference &var) {
  cout << "Visiting " << var.name << " " << &var << endl;
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

any VariableResolver::visit(FunctionCall &funcCall) {
  auto functions = currentScope->getAllFunctionsWithName(funcCall.callee);
  if (functions.empty())
    throw runtime_error("Function " + funcCall.callee + " not found");

  for (auto &function : functions) {
    funcCall.callCandidates.push_back(function->mangledName);
  }

  return {};
}