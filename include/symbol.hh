#pragma once

#include "ast/type.hh"
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
};

struct FunctionSymbol : public Symbol {
  string name;
  string mangledName;
  shared_ptr<Type> returnType;
  vector<shared_ptr<Type>> parameterTypes;
};