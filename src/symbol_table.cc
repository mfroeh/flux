#include "symbol_table.hh"
#include <cassert>
#include <format>

using namespace std;

ostream &operator<<(ostream &os, const SymbolTable &table) {
  os << "Variables:\n";
  for (auto &[mangledName, variable] : table.variables) {
    os << "  " << *variable << '\n';
  }

  os << "Functions:\n";
  for (auto &[mangledName, function] : table.functions) {
    os << "  " << *function << '\n';
  }

  return os;
}

void SymbolTable::insert(shared_ptr<VariableSymbol> variable) {
  assert(!variables.contains(variable->mangledName));
  variables[variable->mangledName] = variable;
}

shared_ptr<VariableSymbol> SymbolTable::lookupVariable(string mangledName) {
  if (!mangledName.starts_with("#")) {
    cout << *this << endl;
    cout << "mangledName: " << mangledName << endl;
    assert(false && "mangledName must start with #");
  } else {
  }

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
  if (!mangledName.starts_with("#") && mangledName != "main") {
    cout << *this << endl;
    assert(false && "mangledName must start with #");
  }

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