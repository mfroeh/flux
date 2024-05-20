#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <memory>
#include <ostream>
#include <string>

using std::shared_ptr;
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
  shared_ptr<struct Type> type;

  // set during codegen
  llvm::AllocaInst *alloc = nullptr;

  VariableSymbol(string name, string mangledName, shared_ptr<Type> type,
                 int depth, int count);

  friend std::ostream &operator<<(std::ostream &os,
                                  const VariableSymbol &symbol);
};

struct FunctionSymbol : public Symbol {
  string name;
  string mangledName;
  shared_ptr<struct Type> returnType;
  vector<shared_ptr<struct Parameter>> parameters;

  // set during codegen
  llvm::Function *llvmFunction = nullptr;

  FunctionSymbol(string name, string mangledName, shared_ptr<Type> returnType,
                 const vector<Parameter> &parameters, int depth, int count);

  friend std::ostream &operator<<(std::ostream &os,
                                  const FunctionSymbol &symbol);
};

struct ClassSymbol : public Symbol {
  string name;
  shared_ptr<Type> type;

  // set during codegen
  llvm::StructType *llvmType = nullptr;

  ClassSymbol(string name, shared_ptr<Type> type);
};