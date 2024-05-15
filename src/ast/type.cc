#include "ast/type.hh"
#include "ir_visitor.hh"
#include <ostream>

using namespace std;

Type::Type(Kind kind) : kind(std::move(kind)) {}

ostream &operator<<(ostream &os, const Type &type) {
  switch (type.kind) {
  case Type::INFER:
    os << "Infer";
    break;
  case Type::ARRAY:
    os << "Array(" << *static_cast<const ArrayType &>(type).elementType << ", "
       << static_cast<const ArrayType &>(type).size << ")";
    break;
  case Type::INT:
    os << "Int";
    break;
  case Type::FLOAT:
    os << "Float";
    break;
  case Type::BOOL:
    os << "Bool";
    break;
  case Type::STRING:
    os << "String";
    break;
  }
  return os;
}

InferType::InferType() : Type(INFER) {}

llvm::Type *InferType::codegen(IRVisitor &visitor) { return nullptr; }

ArrayType::ArrayType(shared_ptr<Type> elementType, long size)
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
  throw std::runtime_error("String type not implemented");
}