#pragma once

#include "symbol.hh"
#include <unordered_map>

using std::string;
using std::unordered_map;

class SymbolTable {
public:
  SymbolTable() = default;

  void insert(shared_ptr<VariableSymbol> variable);
  shared_ptr<VariableSymbol> lookupVariable(string mangledName);

  void insert(shared_ptr<FunctionSymbol> function);
  shared_ptr<FunctionSymbol> lookupFunction(string mangledName);

protected:
  // mangledName, symbol
  unordered_map<string, shared_ptr<VariableSymbol>> variables;
  // mangledName, symbol
  unordered_map<string, shared_ptr<FunctionSymbol>> functions;
};