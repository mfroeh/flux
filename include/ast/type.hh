#pragma once

#include "symbol.hh"
#include "symbol_table.hh"
#include <boost/functional/hash.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <ostream>

using std::shared_ptr;
using std::string;
using std::vector;

struct Type {
  enum Kind {
    VOID,
    INFER,
    ARRAY,
    INT,
    FLOAT,
    BOOL,
    STRING,
    POINTER,
    CLASS,
  } kind;

  Kind Void = VOID;
  Kind Infer = INFER;
  Kind Array = ARRAY;
  Kind Int = INT;
  Kind Float = FLOAT;
  Kind Bool = BOOL;
  Kind String = STRING;
  Kind Pointer = POINTER;
  Kind Class = CLASS;

  Type(Kind kind);
  virtual ~Type() = default;

  bool isInfer() const;
  bool isVoid() const;
  bool isNumber() const;
  bool isInt() const;
  bool isFloat() const;
  bool isBool() const;
  bool isString() const;
  bool isArray() const;
  bool isPointer() const;
  bool isClass() const;

  virtual bool canImplicitlyConvertTo(shared_ptr<Type> other) = 0;

  virtual llvm::Type *codegen(class IRVisitor &visitor) = 0;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) = 0;

  virtual bool canDefaultInitialize() const = 0;
  virtual llvm::Value *getDefaultValue(class IRVisitor &visitor) = 0;
};

std::ostream &operator<<(std::ostream &os, const Type &type);

struct VoidType : public Type {
  static shared_ptr<VoidType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  VoidType();
};

struct InferType : public Type {
  static shared_ptr<InferType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  InferType();
};

struct PointerType : public Type {
  shared_ptr<Type> pointee;

  static shared_ptr<PointerType> get(const shared_ptr<Type> &pointee);
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  PointerType(shared_ptr<Type> pointee);
};

struct ArrayType : public Type {
  shared_ptr<Type> elementType;
  long size;

  static shared_ptr<ArrayType> get(shared_ptr<Type> elementType, long size);
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;

  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  ArrayType(shared_ptr<Type> elementType, long size);
};

struct IntType : public Type {
  static shared_ptr<IntType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  IntType();
};

struct FloatType : public Type {
  static shared_ptr<FloatType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  FloatType();
};

struct BoolType : public Type {
  static shared_ptr<BoolType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;
  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  BoolType();
};

struct StringType : public Type {
  static shared_ptr<StringType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;

  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

private:
  StringType();
};

struct ClassType : public Type {
  string name;

  static shared_ptr<ClassType> get(string name);
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvertTo(shared_ptr<Type> other) override;

  virtual llvm::Value *castTo(llvm::Value *value, shared_ptr<Type> to,
                              class IRVisitor &visitor) override;
  virtual bool canDefaultInitialize() const override;
  llvm::Value *getDefaultValue(class IRVisitor &visitor) override;

  llvm::Value *getFieldPtr(llvm::Value *object, string name,
                           class IRVisitor &visitor);
  llvm::Value *getFieldValue(llvm::Value *object, string name,
                             class IRVisitor &visitor);

  void addField(shared_ptr<VariableSymbol> field);
  shared_ptr<VariableSymbol> getField(string name);
  shared_ptr<Type> getFieldType(string name);

  void addMethod(shared_ptr<FunctionSymbol> method);
  vector<shared_ptr<FunctionSymbol>> getMethods(string name);

private:
  ClassType(string name);

  vector<shared_ptr<VariableSymbol>> fields;
  vector<shared_ptr<FunctionSymbol>> methods;
};