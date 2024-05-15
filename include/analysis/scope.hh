#pragma once

#include "ast/function.hh"
#include "symbol.hh"
#include <unordered_map>
#include <vector>

using std::optional;
using std::shared_ptr;
using std::unordered_map;
using std::vector;

struct Scope {
  Scope(shared_ptr<Scope> parent);

  static int uniqueCounter;

  vector<shared_ptr<VariableSymbol>> variables;
  vector<shared_ptr<FunctionSymbol>> functions;

  void addVariable(string name, shared_ptr<Type> type);
  void addFunction(string name, shared_ptr<Type> returnType,
                   vector<Parameter> parameters);

  shared_ptr<VariableSymbol> getVariable(string name);
  shared_ptr<FunctionSymbol> getFunction(string name);
  vector<shared_ptr<FunctionSymbol>> getAllFunctionsWithName(string name);

  // todo
public:
  shared_ptr<Scope> parent;

  unordered_map<string, int> variableCounts;
  unordered_map<string, int> functionCounts;
  int depth = 0;
  int counter = 0;
};