#pragma once

#include "symbol.hh"
#include <unordered_map>
#include <vector>

using std::optional;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

struct Scope {
  Scope(shared_ptr<Scope> parent = nullptr);

  vector<VariableSymbol> variables;
  vector<FunctionSymbol> functions;

  void addVariable(string name, shared_ptr<Type> type);
  void addFunction(string name, shared_ptr<Type> returnType,
                   vector<shared_ptr<Type>> parameterTypes);

  shared_ptr<VariableSymbol> getVariable(string name);
  shared_ptr<FunctionSymbol> getFunction(string name);

private:
  shared_ptr<Scope> parent;

  unordered_map<string, int> variableCounts;
  unordered_map<string, int> functionCounts;
  int depth = 0;
};