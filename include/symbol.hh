#pragma once

#include "ast/function.hh"
#include "ast/type.hh"
#include <llvm/IR/Instructions.h>
#include <ostream>
#include <string>
using std::string;
using std::vector;

struct Symbol {
  int depth;
  int count;

  Symbol(int depth, int count) : depth(depth), count(count) {}
  virtual ~Symbol() = default;
};

struct VariableSymbol : public Symbol {
  string name;
  string mangledName;
  shared_ptr<Type> type;

  // set during codegen
  llvm::AllocaInst *alloc = nullptr;

  VariableSymbol(string name, string mangledName, shared_ptr<Type> type,
                 int depth, int count)
      : Symbol(depth, count), name(std::move(name)),
        mangledName(std::move(mangledName)), type(std::move(type)) {}

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

  // set during codegen
  llvm::Function *llvmFunction = nullptr;

  FunctionSymbol(string name, string mangledName, shared_ptr<Type> returnType,
                 vector<Parameter> parameters, int depth, int count)
      : Symbol(depth, count), name(std::move(name)),
        mangledName(std::move(mangledName)), returnType(std::move(returnType)),
        parameters(std::move(parameters)) {}

  friend std::ostream &operator<<(std::ostream &os,
                                  const FunctionSymbol &symbol) {
    os << "FunctionSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
       << *symbol.returnType << ", [";
    for (auto &param : symbol.parameters) {
      os << param.name << ": " << *param.type << ", ";
    }
    os << "], "
       << "-> " << *symbol.returnType << ")";
    return os;
  }
};