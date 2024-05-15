#include "analysis/scope.hh"
#include "ast/function.hh"
#include "symbol.hh"
#include <format>
#include <iostream>
#include <ranges>

using namespace std;

Scope::Scope(shared_ptr<Scope> parent) : parent(std::move(parent)) {
  if (this->parent)
    depth = this->parent->depth + 1;
}

void Scope::addVariable(string name, shared_ptr<Type> type) {
  int count = variableCounts[name]++;

  VariableSymbol variable;
  variable.mangledName = std::format("#{}?{}:{}", depth, count, name);
  variable.name = name;
  variable.type = type;
  cout << "inserting " << variable << endl;
  variables.push_back(make_shared<VariableSymbol>(variable));
}

void Scope::addFunction(string name, shared_ptr<Type> returnType,
                        vector<Parameter> parameters) {
  int count = functionCounts[name]++;

  FunctionSymbol function;
  function.mangledName = std::format("@{}?{}:{}", depth, count, name);
  function.name = name;
  function.returnType = returnType;
  function.parameters = parameters;
  cout << "inserting " << function << endl;
  functions.push_back(make_shared<FunctionSymbol>(function));
}

shared_ptr<VariableSymbol> Scope::getVariable(string name) {
  Scope *scope = this;
  cout << "ok" << endl;
  do {
    for (auto &variable : ranges::views::reverse(scope->variables)) {
      if (variable->name == name)
        return variable;
    }
    scope = scope->parent.get();
  } while (scope);
  return nullptr;
}

shared_ptr<FunctionSymbol> Scope::getFunction(string name) {
  Scope *scope = this;
  do {
    for (auto &function : ranges::views::reverse(scope->functions)) {
      if (function->name == name)
        return function;
    }
    scope = scope->parent.get();
  } while (scope);
  return nullptr;
}
