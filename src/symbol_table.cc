#include "symbol_table.hh"
#include <cassert>

void SymbolTable::insert(shared_ptr<VariableSymbol> variable) {
  assert(!variables.contains(variable->mangledName));
  variables[variable->mangledName] = variable;
}

shared_ptr<VariableSymbol> SymbolTable::lookupVariable(string mangledName) {
  if (variables.contains(mangledName)) {
    return variables[mangledName];
  }
  return nullptr;
}

void SymbolTable::insert(shared_ptr<FunctionSymbol> function) {
  assert(!functions.contains(function->mangledName));
  functions[function->mangledName] = function;
}

shared_ptr<FunctionSymbol> SymbolTable::lookupFunction(string mangledName) {
  if (functions.contains(mangledName)) {
    return functions[mangledName];
  }
  return nullptr;
}
