#include "analysis/variable_resolver.hh"
#include "ast/class.hh"
#include "ast/expr.hh"
#include "ast/stmt.hh"
#include "symbol_table.hh"
#include "visitor.hh"
#include <format>
#include <sstream>

using namespace std;

VariableResolver::VariableResolver(ModuleContext &context, SymbolTable &symTab)
    : AstVisitor(context, symTab) {
  currentScope = make_shared<Scope>(nullptr);
}

any VariableResolver::visit(ClassDefinition &classDefinition) {
  auto previousScope = currentScope;
  currentScope = make_shared<Scope>(previousScope);

  AstVisitor::visit(classDefinition);

  cout << "Adding class " << classDefinition.type->name << endl;
  for (auto &field : classDefinition.fields) {
    // we are in the class scope here
    auto symbol = currentScope->getVariableNonRecursive(field.name);
    assert(symbol);

    string mangledName =
        std::format("#{}.${}", classDefinition.type->name, field.name);

    symbol->mangledName = mangledName;
    field.mangledName = mangledName;

    cout << "Added field" << field.name << endl;
    classDefinition.type->addField(symbol);
  }

  for (auto &method : classDefinition.methods) {
    auto symbol = symTab.lookupFunction(method.mangledName);
    assert(symbol);
    string oldMangledName = symbol->mangledName;

    // Make mangled name
    stringstream ss;
    ss << "#" << classDefinition.type->name << ".$" << method.name << "?";
    for (auto &param : method.parameters)
      ss << *param.type << "?";
    symbol->mangledName = ss.str();
    method.mangledName = ss.str();

    // Readd the new function with the correctly mangledName
    symTab.removeFunction(oldMangledName);
    symTab.insert(symbol);

    classDefinition.type->addMethod(symbol);
  }

  auto symbol = make_shared<ClassSymbol>(classDefinition.type->name,
                                         classDefinition.type);
  symTab.insert(symbol);

  currentScope = previousScope;
  return {};
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

  AstVisitor::visit(block);

  currentScope = previousScope;
  return {};
}

any VariableResolver::visit(Parameter &parameter) {
  if (currentScope->getVariableNonRecursive(parameter.name) != nullptr) {
    throw runtime_error("Parameter " + parameter.name + " already declared");
  }

  currentScope->addVariable(parameter.name, parameter.type);
  auto variable = currentScope->getVariable(parameter.name);
  symTab.insert(variable);
  parameter.mangledName = variable->mangledName;
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

any VariableResolver::visit(FieldDeclaration &fieldDeclaration) {
  if (currentScope->getVariableNonRecursive(fieldDeclaration.name) != nullptr)
    throw runtime_error("Field " + fieldDeclaration.name + " already declared");

  currentScope->addVariable(fieldDeclaration.name, fieldDeclaration.type);

  // TODO: probably don't need this
  auto variable = currentScope->getVariable(fieldDeclaration.name);
  symTab.insert(variable);

  return {};
}

any VariableResolver::visit(VarRef &var) {
  auto variable = currentScope->getVariable(var.name);
  if (!variable)
    throw runtime_error("Variable " + var.name + " not found");

  var.mangledName = variable->mangledName;
  return {};
}

any VariableResolver::visit(ArrayRef &arr) {
  AstVisitor::visit(arr);
  // TODO: but we shouldn't need this
  // arr.index->accept(*this);
  // auto variableRef = dynamic_pointer_cast<VariableReference>(arr.arrayExpr);
  // if (variableRef == nullptr)
  //   return {};

  // string name = variableRef->name;
  // auto variable = currentScope->getVariable(name);
  // if (!variable)
  //   throw runtime_error("Variable " + name + " not found");

  // variableRef->mangledName = variable->mangledName;
  return {};
}

any VariableResolver::visit(FunctionCall &funcCall) {
  AstVisitor::visit(funcCall);

  auto functions = currentScope->getAllFunctionsWithName(funcCall.callee);
  if (functions.empty())
    throw runtime_error("Function " + funcCall.callee + " not found");

  for (auto &function : functions) {
    funcCall.callCandidates.push_back(function->mangledName);
  }

  return {};
}