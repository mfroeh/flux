#include "symbol.hh"
#include "ast/function.hh"
#include "ast/type.hh"

std::ostream &operator<<(std::ostream &os, const VariableSymbol &symbol) {
  os << "VariableSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
     << *symbol.type << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const FunctionSymbol &symbol) {
  os << "FunctionSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
     << *symbol.returnType << ", [";
  for (auto &param : symbol.parameters) {
    os << param.name << ": " << *param.type << ", ";
  }
  os << "], "
     << "-> " << *symbol.returnType << ")";
  return os;
}