#include "analysis/scope.hh"
#include "ast/function.hh"
#include "symbol.hh"
#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>

using namespace std;

int Scope::uniqueCounter = 0;

Scope::Scope(shared_ptr<Scope> parent) : parent(std::move(parent)) {
  if (this->parent) {
    depth = this->parent->depth + 1;
  }
  counter = uniqueCounter++;
}

void Scope::addVariable(string name, shared_ptr<Type> type) {
  int count = variableCounts[name]++;

  auto mangledName = std::format("#d{}n{}c{}:{}", depth, count, counter, name);
  auto variable = VariableSymbol(name, mangledName, type, depth, count);
  variables.push_back(make_shared<VariableSymbol>(variable));
}

void Scope::addFunction(string name, shared_ptr<Type> returnType,
                        vector<Parameter> parameters) {
  int count = functionCounts[name]++;

  if (name == "main" && count > 0)
    throw runtime_error("main function can only be defined once");

  auto mangledName = std::format("#d{}n{}c{}:{}", depth, count, counter, name);
  if (name == "main")
    mangledName = "main";

  auto function =
      FunctionSymbol(name, mangledName, returnType, parameters, depth, count);
  functions.push_back(make_shared<FunctionSymbol>(function));
}

shared_ptr<VariableSymbol> Scope::getVariable(string name) {
  Scope *scope = this;
  do {
    for (auto &variable : ranges::views::reverse(scope->variables)) {
      if (variable->name == name)
        return variable;
    }
    scope = scope->parent.get();
  } while (scope);
  return nullptr;
}

shared_ptr<VariableSymbol> Scope::getVariableNonRecursive(string name) {
  for (auto &variable : variables) {
    if (variable->name == name)
      return variable;
  }
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

vector<shared_ptr<FunctionSymbol>> Scope::getAllFunctionsWithName(string name) {
  vector<shared_ptr<FunctionSymbol>> result;
  Scope *scope = this;
  do {
    for (auto &function : ranges::views::reverse(scope->functions)) {
      if (function->name == name)
        result.push_back(function);
    }
    scope = scope->parent.get();
  } while (scope);
  return result;
}