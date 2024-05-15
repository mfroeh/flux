#include "ast/type.hh"
#include "ir_visitor.hh"
#include <ostream>

using namespace std;

Type::Type(Kind kind) : kind(std::move(kind)) {}

bool Type::isInfer() const { return kind == INFER; }
bool Type::isVoid() const { return false; }
bool Type::isNumber() const { return isInt() || isFloat(); }
bool Type::isInt() const { return kind == INT; }
bool Type::isFloat() const { return kind == FLOAT; }
bool Type::isBool() const { return kind == BOOL; }
bool Type::isString() const { return kind == STRING; }
bool Type::isArray() const { return kind == ARRAY; }

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

shared_ptr<InferType> InferType::get() {
  static shared_ptr<InferType> instance =
      shared_ptr<InferType>(new InferType());
  return instance;
}

llvm::Type *InferType::codegen(IRVisitor &visitor) { return nullptr; }

bool InferType::canImplicitlyConvert(shared_ptr<Type> other) { return true; }

ArrayType::ArrayType(shared_ptr<Type> elementType, long size)
    : Type(ARRAY), elementType(std::move(elementType)), size(size) {}

shared_ptr<ArrayType> ArrayType::get(shared_ptr<Type> elementType, long size) {
  static std::unordered_map<std::pair<shared_ptr<Type>, long>,
                            shared_ptr<ArrayType>,
                            boost::hash<std::pair<shared_ptr<Type>, long>>>
      instances;

  auto pair = make_pair(elementType, size);
  if (!instances.contains(pair)) {
    instances[pair] = shared_ptr<ArrayType>(new ArrayType(elementType, size));
  }
  return instances[pair];
}

llvm::Type *ArrayType::codegen(IRVisitor &visitor) {
  return llvm::ArrayType::get(elementType->codegen(visitor), size);
}

bool ArrayType::canImplicitlyConvert(shared_ptr<Type> other) {
  return other.get() == this;
}

IntType::IntType() : Type(INT) {}

shared_ptr<IntType> IntType::get() {
  static shared_ptr<IntType> instance = shared_ptr<IntType>(new IntType());
  return instance;
}

llvm::Type *IntType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt64Ty(visitor.context);
}

bool IntType::canImplicitlyConvert(shared_ptr<Type> other) {
  return other->isInt() || other->isBool() || other->isFloat();
}

FloatType::FloatType() : Type(FLOAT) {}

shared_ptr<FloatType> FloatType::get() {
  static shared_ptr<FloatType> instance =
      shared_ptr<FloatType>(new FloatType());
  return instance;
}

llvm::Type *FloatType::codegen(IRVisitor &visitor) {
  return llvm::Type::getDoubleTy(visitor.context);
}

bool FloatType::canImplicitlyConvert(shared_ptr<Type> other) {
  return other->isFloat();
}

BoolType::BoolType() : Type(BOOL) {}

shared_ptr<BoolType> BoolType::get() {
  static shared_ptr<BoolType> instance = shared_ptr<BoolType>(new BoolType());
  return instance;
}

llvm::Type *BoolType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt1Ty(visitor.context);
}

bool BoolType::canImplicitlyConvert(shared_ptr<Type> other) {
  return other->isBool();
}

StringType::StringType() : Type(STRING) {}

shared_ptr<StringType> StringType::get() {
  static shared_ptr<StringType> instance =
      shared_ptr<StringType>(new StringType());
  return instance;
}

bool StringType::canImplicitlyConvert(shared_ptr<Type> other) {
  throw std::runtime_error("String type not implemented");
}

llvm::Type *StringType::codegen(IRVisitor &visitor) {
  // return llvm::Type::getInt8PtrTy(visitor.context);
  throw std::runtime_error("String type not implemented");
}
