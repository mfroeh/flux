#pragma once

#include <boost/functional/hash.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <ostream>

using std::shared_ptr;
using std::string;

struct Type {
  enum Kind {
    INFER,
    ARRAY,
    INT,
    FLOAT,
    BOOL,
    STRING,
  } kind;

  Kind Infer = INFER;
  Kind Array = ARRAY;
  Kind Int = INT;
  Kind Float = FLOAT;
  Kind Bool = BOOL;
  Kind String = STRING;

  Type(Kind kind);
  virtual ~Type() = default;

  bool isInfer() const;
  bool isNumber() const;
  bool isInt() const;
  bool isFloat() const;
  bool isBool() const;
  bool isString() const;
  bool isArray() const;

  virtual bool canImplicitlyConvert(shared_ptr<Type> other) = 0;

  virtual llvm::Type *codegen(class IRVisitor &visitor) = 0;
};

std::ostream &operator<<(std::ostream &os, const Type &type);

struct InferType : public Type {
  static shared_ptr<InferType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  InferType();
};

struct ArrayType : public Type {
  shared_ptr<Type> elementType;
  long size;

  static shared_ptr<ArrayType> get(shared_ptr<Type> elementType, long size);
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  ArrayType(shared_ptr<Type> elementType, long size);
};

struct IntType : public Type {
  static shared_ptr<IntType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  IntType();
};

struct FloatType : public Type {
  static shared_ptr<FloatType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  FloatType();
};

struct BoolType : public Type {
  static shared_ptr<BoolType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  BoolType();
};

struct StringType : public Type {
  static shared_ptr<StringType> get();
  llvm::Type *codegen(class IRVisitor &visitor) override;

  bool canImplicitlyConvert(shared_ptr<Type> other) override;

private:
  StringType();
};