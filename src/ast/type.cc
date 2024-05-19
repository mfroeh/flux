#include "ast/type.hh"
#include "ast/expr.hh"
#include "codegen/ir_visitor.hh"
#include "symbol.hh"
#include <algorithm>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
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

llvm::Value *VoidType::getDefaultValue(IRVisitor &visitor) { assert(false); }

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

llvm::Value *InferType::getDefaultValue(IRVisitor &visitor) { assert(false); }

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

llvm::Value *PointerType::getDefaultValue(IRVisitor &visitor) { assert(false); }

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

bool ArrayType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  if (other.get() == this)
    return true;

  cout << *other << endl;

  if (!other->isArray())
    return false;

  auto otherArray = static_pointer_cast<ArrayType>(other);
  if (otherArray->size < size)
    return false;

  // required matching element types (relax to implicit conversion later)
  if (otherArray->elementType != elementType)
    return false;

  // if elements have to be added
  if (size < otherArray->size)
    return otherArray->elementType->canDefaultInitialize();

  return true;
}

llvm::Value *ArrayType::castTo(llvm::Value *loadedArr, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  assert(canImplicitlyConvertTo(to));
  if (to.get() == this)
    return loadedArr;

  auto oldSize = size;
  auto oldElementType = elementType;
  auto oldElemTypeLlvm = oldElementType->codegen(visitor);

  auto newSize = static_pointer_cast<ArrayType>(to)->size;
  auto newElementType = static_pointer_cast<ArrayType>(to)->elementType;
  auto newElemTypeLlvm = newElementType->codegen(visitor);

  assert(oldSize <= newSize);
  // todo: maybe relax later
  assert(oldElementType == newElementType);

  auto &builder = visitor.builder;

  // new array
  auto newAlloca =
      visitor.builder->CreateAlloca(to->codegen(visitor), nullptr, "arrayCast");

  // copy over old elements
  for (unsigned i = 0; i < oldSize; i++) {
    auto oldElement = builder->CreateExtractValue(loadedArr, {i}, "oldElem");

    auto index =
        llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
    auto newElementPtr = builder->CreateInBoundsGEP(newElemTypeLlvm, newAlloca,
                                                    index, "newElemPtr");
    builder->CreateStore(oldElement, newElementPtr);
  }

  // fill with rest
  auto defaultVal = newElementType->getDefaultValue(visitor);
  llvm::outs() << *defaultVal;
  for (int i = oldSize; i < newSize; i++) {
    auto index =
        llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
    auto elemPtr = builder->CreateInBoundsGEP(newElemTypeLlvm, newAlloca, index,
                                              "newElemPtr");
    builder->CreateStore(defaultVal, elemPtr);
  }

  // todo: should we also load here?
  return builder->CreateLoad(to->codegen(visitor), newAlloca, "arrayCast");
}

bool ArrayType::canDefaultInitialize() const {
  return elementType->canDefaultInitialize();
}

llvm::Value *ArrayType::getDefaultValue(IRVisitor &visitor) {
  auto &builder = visitor.builder;
  auto alloca = builder->CreateAlloca(codegen(visitor), nullptr, "tmpArray");
  for (unsigned i = 0; i < size; i++) {
    auto index =
        llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
    auto elemPtr = builder->CreateInBoundsGEP(elementType->codegen(visitor),
                                              alloca, index, "elemPtr");
    builder->CreateStore(elementType->getDefaultValue(visitor), elemPtr);
  }
  return builder->CreateLoad(codegen(visitor), alloca, "tmpArray");
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

llvm::Value *IntType::getDefaultValue(IRVisitor &visitor) {
  return llvm::ConstantInt::get(codegen(visitor), 0);
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

llvm::Value *FloatType::getDefaultValue(IRVisitor &visitor) {
  return llvm::ConstantFP::get(codegen(visitor), 0.0);
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

llvm::Value *BoolType::getDefaultValue(IRVisitor &visitor) {
  return llvm::ConstantInt::get(codegen(visitor), 0);
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

llvm::Value *StringType::getDefaultValue(IRVisitor &visitor) {
  throw std::runtime_error("String type not implemented");
}

ClassType::ClassType(string name) : Type(CLASS), name(std::move(name)) {}

shared_ptr<ClassType> ClassType::get(string name) {
  static std::unordered_map<string, shared_ptr<ClassType>> instances;

  if (!instances.contains(name)) {
    instances[name] = shared_ptr<ClassType>(new ClassType(name));
  }
  return instances[name];
}

llvm::Type *ClassType::codegen(IRVisitor &visitor) {
  auto type = llvm::StructType::getTypeByName(*visitor.llvmContext, name);
  assert(type && "class type not found");
  return type;
}

bool ClassType::canImplicitlyConvertTo(shared_ptr<Type> other) { return false; }

llvm::Value *ClassType::castTo(llvm::Value *value, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  assert(false);
}

bool ClassType::canDefaultInitialize() const {
  assert(fields.size() > 0);
  for (auto &field : fields) {
    if (!field->type->canDefaultInitialize())
      return false;
  }
  return true;
}

llvm::Value *ClassType::getDefaultValue(IRVisitor &visitor) {
  assert(canDefaultInitialize());

  auto type = codegen(visitor);
  auto alloca = visitor.builder->CreateAlloca(type, nullptr, "classDefault");

  int fieldCount = type->getStructNumElements();
  for (int i = 0; i < fieldCount; i++) {
    auto index =
        llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
    auto fieldType = fields[i]->type->codegen(visitor);
    auto typeDefault = fields[i]->type->getDefaultValue(visitor);

    auto fieldPtr =
        visitor.builder->CreateInBoundsGEP(type, alloca, {index}, "fieldPtr");
    visitor.builder->CreateStore(typeDefault, fieldPtr);
  }

  // todo: maybe just return the alloca?
  return visitor.builder->CreateLoad(type, alloca, "loadClass");
}

llvm::Value *ClassType::getFieldPtr(llvm::Value *object, string name,
                                    IRVisitor &visitor) {
  auto type = codegen(visitor);
  auto fieldIndex = ranges::distance(
      fields.begin(), ranges::find_if(fields, [name](const auto &field) {
        return field->name == name;
      }));

  auto index =
      llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, fieldIndex));
  return visitor.builder->CreateInBoundsGEP(type, object, {index}, "fieldPtr");
}

llvm::Value *ClassType::getFieldValue(llvm::Value *object, string name,
                                      IRVisitor &visitor) {
  auto type = codegen(visitor);
  auto fieldIndex = ranges::distance(
      fields.begin(), ranges::find_if(fields, [name](const auto &field) {
        return field->name == name;
      }));
  return visitor.builder->CreateExtractValue(
      object, {static_cast<unsigned int>(fieldIndex)}, "fieldValue");
}

void ClassType::addField(shared_ptr<VariableSymbol> field) {
  auto existing = ranges::find_if(
      fields, [field](const auto &f) { return f->name == field->name; });
  assert(existing == fields.end());

  fields.push_back(field);
}

shared_ptr<Type> ClassType::getFieldType(string name) {
  return getField(name)->type;
}

shared_ptr<VariableSymbol> ClassType::getField(string name) {
  auto existing = ranges::find_if(
      fields, [name](const auto &f) { return f->name == name; });
  assert(existing != fields.end());

  return *existing;
}

void ClassType::addMethod(shared_ptr<FunctionSymbol> method) {
  methods.push_back(method);
}

vector<shared_ptr<FunctionSymbol>> ClassType::getMethods(string name) {
  vector<shared_ptr<FunctionSymbol>> result;
  ranges::copy_if(methods, back_inserter(result),
                  [name](const auto &method) { return method->name == name; });
  return result;
}