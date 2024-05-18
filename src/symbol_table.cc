#include "symbol_table.hh"
#include <cassert>

void SymbolTable::insert(shared_ptr<VariableSymbol> variable) {
  assert(!variables.contains(variable->mangledName));
  variables[variable->mangledName] = variable;
}

shared_ptr<VariableSymbol> SymbolTable::lookupVariable(string mangledName) {
  if (!mangledName.starts_with("#"))
    assert(false && "mangledName must start with #");

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
  if (!mangledName.starts_with("#") && mangledName != "main")
    assert(false && "mangledName must start with #");

  if (functions.contains(mangledName)) {
    return functions[mangledName];
  }
  return nullptr;
}

vector<shared_ptr<FunctionSymbol>> SymbolTable::getFunctions(string name) {
  vector<shared_ptr<FunctionSymbol>> result;
  for (auto &[mangledName, function] : functions) {
    if (function->name == name) {
      result.push_back(function);
    }
  }
  return result;
}