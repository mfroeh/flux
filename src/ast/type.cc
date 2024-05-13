#include "ast/type.hh"
#include "ir_visitor.hh"

Type::Type(Kind kind) : kind(std::move(kind)) {}

InferType::InferType() : Type(INFER) {}

llvm::Type *InferType::codegen(IRVisitor &visitor) { return nullptr; }

ArrayType::ArrayType(unique_ptr<Type> elementType, long size)
    : Type(ARRAY), elementType(std::move(elementType)), size(size) {}

llvm::Type *ArrayType::codegen(IRVisitor &visitor) {
  return llvm::ArrayType::get(elementType->codegen(visitor), size);
}

IntType::IntType() : Type(INT) {}

llvm::Type *IntType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt64Ty(visitor.context);
}

FloatType::FloatType() : Type(FLOAT) {}

llvm::Type *FloatType::codegen(IRVisitor &visitor) {
  return llvm::Type::getDoubleTy(visitor.context);
}

BoolType::BoolType() : Type(BOOL) {}

llvm::Type *BoolType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt1Ty(visitor.context);
}

StringType::StringType() : Type(STRING) {}

llvm::Type *StringType::codegen(IRVisitor &visitor) {
  // return llvm::Type::getInt8PtrTy(visitor.context);
}