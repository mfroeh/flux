#pragma once

#include "ast/function.hh"
#include "ast/type.hh"
#include <ostream>
#include <string>
using std::string;
using std::vector;

struct Symbol {
  virtual ~Symbol() = default;
};

struct VariableSymbol : public Symbol {
  string name;
  string mangledName;
  shared_ptr<Type> type;

  friend std::ostream &operator<<(std::ostream &os,
                                  const VariableSymbol &symbol) {
    os << "VariableSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
       << *symbol.type << ")";
    return os;
  }
};

struct FunctionSymbol : public Symbol {
  string name;
  string mangledName;
  shared_ptr<Type> returnType;
  vector<Parameter> parameters;

  friend std::ostream &operator<<(std::ostream &os,
                                  const FunctionSymbol &symbol) {
    os << "FunctionSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
       << *symbol.returnType << ", [";
    for (auto &type : symbol.parameters) {
      os << type << ", ";
    }
    os << "])";
    return os;
  }
};