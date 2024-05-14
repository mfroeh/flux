#include "analysis/scope.hh"
#include "symbol.hh"
#include <format>
#include <ranges>

using namespace std;

Scope::Scope(shared_ptr<Scope> parent) : parent(std::move(parent)) {
  if (parent)
    depth = parent->depth + 1;
}

void Scope::addVariable(string name, shared_ptr<Type> type) {
  int count = ++variableCounts[name];

  VariableSymbol variable;
  variable.mangledName = std::format("#{}?{}:{}", depth, count, name);
  variable.name = name;
  variable.type = type;
  variables.push_back(variable);
}

void Scope::addFunction(string name, shared_ptr<Type> returnType,
                        vector<shared_ptr<Type>> parameterTypes) {
  int count = ++functionCounts[name];

  FunctionSymbol function;
  function.mangledName = std::format("@{}?{}:{}", depth, count, name);
  function.name = name;
  function.returnType = returnType;
  function.parameterTypes = parameterTypes;
  functions.push_back(function);
}

shared_ptr<VariableSymbol> Scope::getVariable(string name) {
  Scope *scope = this;
  do {
    for (auto &variable : ranges::views::reverse(scope->variables)) {
      if (variable.name == name)
        return shared_ptr<VariableSymbol>(&variable);
    }
    scope = scope->parent.get();
  } while (scope);
  return nullptr;
}

shared_ptr<FunctionSymbol> Scope::getFunction(string name) {
  Scope *scope = this;
  do {
    for (auto &function : ranges::views::reverse(scope->functions)) {
      if (function.name == name)
        return shared_ptr<FunctionSymbol>(&function);
    }
    scope = scope->parent.get();
  } while (scope);
  return nullptr;
}
