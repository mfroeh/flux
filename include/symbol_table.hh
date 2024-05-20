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

  void insert(shared_ptr<ClassSymbol> classSymbol);
  shared_ptr<ClassSymbol> lookupClass(string name);

  vector<shared_ptr<FunctionSymbol>> getFunctions(string name);

  // when replacing method calls with function calls
  void removeFunction(string mangledName);

  friend std::ostream &operator<<(std::ostream &os, const SymbolTable &table);

protected:
  // mangledName, symbol
  unordered_map<string, shared_ptr<VariableSymbol>> variables;
  // mangledName, symbol
  unordered_map<string, shared_ptr<FunctionSymbol>> functions;
  // name, class
  unordered_map<string, shared_ptr<ClassSymbol>> classes;
};