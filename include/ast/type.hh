#pragma once

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

  virtual llvm::Type *codegen(class IRVisitor &visitor) = 0;
};

std::ostream &operator<<(std::ostream &os, const Type &type);

struct InferType : public Type {
  InferType();

  llvm::Type *codegen(class IRVisitor &visitor) override;
};

struct ArrayType : public Type {
  shared_ptr<Type> elementType;
  long size;

  ArrayType(shared_ptr<Type> elementType, long size);

  llvm::Type *codegen(class IRVisitor &visitor) override;
};

struct IntType : public Type {
  IntType();
  llvm::Type *codegen(class IRVisitor &visitor) override;
};

struct FloatType : public Type {
  FloatType();
  llvm::Type *codegen(class IRVisitor &visitor) override;
};

struct BoolType : public Type {
  BoolType();
  llvm::Type *codegen(class IRVisitor &visitor) override;
};

struct StringType : public Type {
  StringType();
  llvm::Type *codegen(class IRVisitor &visitor) override;
};