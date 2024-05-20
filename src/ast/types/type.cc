#include "ast/type.hh"
#include "ast/expr.hh"
#include "codegen/ir_visitor.hh"
#include "symbol.hh"
#include <algorithm>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <ostream>
#include <ranges>

using namespace std;

Type::Type(Kind kind) : kind(std::move(kind)) {}

bool Type::isInfer() const { return kind == INFER; }
bool Type::isVoid() const { return kind == VOID; }
bool Type::isNumber() const { return isInt() || isFloat(); }
bool Type::isInt() const { return kind == INT; }
bool Type::isFloat() const { return kind == FLOAT; }
bool Type::isBool() const { return kind == BOOL; }
bool Type::isString() const { return kind == STRING; }
bool Type::isArray() const { return kind == ARRAY; }
bool Type::isPointer() const { return kind == POINTER; }
bool Type::isClass() const { return kind == CLASS; }

ostream &operator<<(ostream &os, const Type &type) {
  switch (type.kind) {
  case Type::INFER:
    os << "infer";
    break;
  case Type::ARRAY:
    os << "Array(" << *static_cast<const ArrayType &>(type).elementType << ", "
       << static_cast<const ArrayType &>(type).size << ")";
    break;
  case Type::INT:
    os << "int";
    break;
  case Type::FLOAT:
    os << "float";
    break;
  case Type::BOOL:
    os << "bool";
    break;
  case Type::STRING:
    os << "str";
    break;
  case Type::POINTER:
    os << "Pointer(" << *static_cast<const PointerType &>(type).pointee << ")";
    break;
  case Type::VOID:
    os << "void";
    break;
  case Type::CLASS:
    os << "Class(" << static_cast<const ClassType &>(type).name << ")";
    break;
  }
  return os;
}

VoidType::VoidType() : Type(VOID) {}

shared_ptr<VoidType> VoidType::get() {
  static shared_ptr<VoidType> instance = shared_ptr<VoidType>(new VoidType());
  return instance;
}

llvm::Type *VoidType::codegen(IRVisitor &visitor) {
  return llvm::Type::getVoidTy(*visitor.llvmContext);
}

bool VoidType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other->isVoid();
}

llvm::Value *VoidType::castTo(llvm::Value *value, shared_ptr<Type> to,
                              IRVisitor &visitor) {
  assert(false);
}

bool VoidType::canDefaultInitialize() const { assert(false); }

llvm::Value *VoidType::defaultInitialize(llvm::Value *alloca,
                                         IRVisitor &visitor) {
  assert(false);
}

InferType::InferType() : Type(INFER) {}

shared_ptr<InferType> InferType::get() {
  static shared_ptr<InferType> instance =
      shared_ptr<InferType>(new InferType());
  return instance;
}

llvm::Type *InferType::codegen(IRVisitor &visitor) { assert(false); }

bool InferType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return !other->isVoid();
}

llvm::Value *InferType::castTo(llvm::Value *value, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  assert(false);
}

bool InferType::canDefaultInitialize() const { assert(false); }

llvm::Value *InferType::defaultInitialize(llvm::Value *alloca,
                                          IRVisitor &visitor) {
  assert(false);
}

PointerType::PointerType(shared_ptr<Type> pointee)
    : Type(POINTER), pointee(std::move(pointee)) {}

shared_ptr<PointerType> PointerType::get(const shared_ptr<Type> &pointee) {
  static std::unordered_map<shared_ptr<Type>, shared_ptr<PointerType>>
      instances;

  if (!instances.contains(pointee)) {
    instances[pointee] = shared_ptr<PointerType>(new PointerType(pointee));
  }
  return instances[pointee];
}

llvm::Type *PointerType::codegen(IRVisitor &visitor) {
  return llvm::PointerType::getUnqual(pointee->codegen(visitor));
}

bool PointerType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other.get() == this;
}

llvm::Value *PointerType::castTo(llvm::Value *value, shared_ptr<Type> to,
                                 IRVisitor &visitor) {
  assert(false);
}

bool PointerType::canDefaultInitialize() const { return false; }

llvm::Value *PointerType::defaultInitialize(llvm::Value *alloca,
                                            IRVisitor &visitor) {
  assert(false);
}

IntType::IntType() : Type(INT) {}

shared_ptr<IntType> IntType::get() {
  static shared_ptr<IntType> instance = shared_ptr<IntType>(new IntType());
  return instance;
}

llvm::Type *IntType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt64Ty(*visitor.llvmContext);
}

bool IntType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other->isInt() || other->isBool() || other->isFloat();
}

llvm::Value *IntType::castTo(llvm::Value *value, shared_ptr<Type> to,
                             IRVisitor &visitor) {
  if (to == this->get())
    return value;

  if (to->isFloat())
    return visitor.builder->CreateSIToFP(value, to->codegen(visitor),
                                         "intToFloat");

  if (to->isBool()) {
    return visitor.builder->CreateICmpNE(
        value, llvm::ConstantInt::get(codegen(visitor), 0), "intToBool");
  }

  throw std::runtime_error("Invalid cast");
}

bool IntType::canDefaultInitialize() const { return true; }

llvm::Value *IntType::defaultInitialize(llvm::Value *alloca,
                                        IRVisitor &visitor) {
  return visitor.builder->CreateStore(
      llvm::ConstantInt::get(codegen(visitor), 0), alloca);
}

FloatType::FloatType() : Type(FLOAT) {}

shared_ptr<FloatType> FloatType::get() {
  static shared_ptr<FloatType> instance =
      shared_ptr<FloatType>(new FloatType());
  return instance;
}

llvm::Type *FloatType::codegen(IRVisitor &visitor) {
  return llvm::Type::getDoubleTy(*visitor.llvmContext);
}

bool FloatType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other->isFloat();
}

llvm::Value *FloatType::castTo(llvm::Value *value, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  if (to == this->get())
    return value;

  if (to->isInt())
    return visitor.builder->CreateFPToSI(value, to->codegen(visitor),
                                         "floatToInt");

  throw std::runtime_error("Invalid cast");
}

bool FloatType::canDefaultInitialize() const { return true; }

llvm::Value *FloatType::defaultInitialize(llvm::Value *alloca,
                                          IRVisitor &visitor) {
  return visitor.builder->CreateStore(
      llvm::ConstantFP::get(codegen(visitor), 0.0), alloca);
}

BoolType::BoolType() : Type(BOOL) {}

shared_ptr<BoolType> BoolType::get() {
  static shared_ptr<BoolType> instance = shared_ptr<BoolType>(new BoolType());
  return instance;
}

llvm::Type *BoolType::codegen(IRVisitor &visitor) {
  return llvm::Type::getInt1Ty(*visitor.llvmContext);
}

bool BoolType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other->isBool();
}

llvm::Value *BoolType::castTo(llvm::Value *value, shared_ptr<Type> to,
                              IRVisitor &visitor) {
  if (to == this->get())
    return value;

  throw std::runtime_error("Invalid cast");
}

bool BoolType::canDefaultInitialize() const { return true; }

llvm::Value *BoolType::defaultInitialize(llvm::Value *alloca,
                                         IRVisitor &visitor) {
  return visitor.builder->CreateStore(
      llvm::ConstantInt::get(codegen(visitor), 0), alloca);
}

StringType::StringType() : Type(STRING) {}

shared_ptr<StringType> StringType::get() {
  static shared_ptr<StringType> instance =
      shared_ptr<StringType>(new StringType());
  return instance;
}

bool StringType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  throw std::runtime_error("String type not implemented");
}

llvm::Type *StringType::codegen(IRVisitor &visitor) {
  // return llvm::Type::getInt8PtrTy(visitor.context);
  throw std::runtime_error("String type not implemented");
}

llvm::Value *StringType::castTo(llvm::Value *value, shared_ptr<Type> to,
                                IRVisitor &visitor) {
  throw std::runtime_error("String type not implemented");
}

bool StringType::canDefaultInitialize() const {
  throw std::runtime_error("String type not implemented");
}

llvm::Value *StringType::defaultInitialize(llvm::Value *alloca,
                                           IRVisitor &visitor) {
  throw std::runtime_error("String type not implemented");
}
