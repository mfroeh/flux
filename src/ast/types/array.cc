#include "ast/type.hh"
#include "codegen/ir_visitor.hh"
#include <iostream>

using namespace std;

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

  if (!other->isArray())
    return false;

  auto otherArray = static_pointer_cast<ArrayType>(other);
  if (otherArray->size < size)
    return false;

  // required matching element types
  if (otherArray->elementType != elementType)
    return false;

  // if elements have to be added
  if (size < otherArray->size)
    return otherArray->elementType->canDefaultInitialize();

  return true;
}

llvm::Value *ArrayType::castTo(llvm::Value *loadedArr, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  assert(false);
  assert(canImplicitlyConvertTo(to));
  // if (to.get() == this)
  //   return loadedArr;

  // auto oldSize = size;
  // auto oldElementType = elementType;
  // auto oldElemTypeLlvm = oldElementType->codegen(visitor);

  // auto newSize = static_pointer_cast<ArrayType>(to)->size;
  // auto newElementType = static_pointer_cast<ArrayType>(to)->elementType;
  // auto newElemTypeLlvm = newElementType->codegen(visitor);

  // assert(oldSize <= newSize);
  // // todo: maybe relax later
  // assert(oldElementType == newElementType);

  // auto &builder = visitor.builder;

  // // new array
  // auto newAlloca =
  //     visitor.builder->CreateAlloca(to->codegen(visitor), nullptr,
  //     "arrayCast");

  // // copy over old elements
  // for (unsigned i = 0; i < oldSize; i++) {
  //   auto oldElement = builder->CreateExtractValue(loadedArr, {i}, "oldElem");

  //   auto index =
  //       llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
  //   auto newElementPtr = builder->CreateInBoundsGEP(newElemTypeLlvm,
  //   newAlloca,
  //                                                   index, "newElemPtr");
  //   builder->CreateStore(oldElement, newElementPtr);
  // }

  // // fill with rest
  // auto defaultVal = newElementType->getDefaultValue(visitor);
  // llvm::outs() << *defaultVal;
  // for (int i = oldSize; i < newSize; i++) {
  //   auto index =
  //       llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
  //   auto elemPtr = builder->CreateInBoundsGEP(newElemTypeLlvm, newAlloca,
  //   index,
  //                                             "newElemPtr");
  //   builder->CreateStore(defaultVal, elemPtr);
  // }

  // // todo: should we also load here?
  // return builder->CreateLoad(to->codegen(visitor), newAlloca, "arrayCast");
}

bool ArrayType::canDefaultInitialize() const {
  return elementType->canDefaultInitialize();
}

llvm::Value *ArrayType::defaultInitialize(llvm::Value *alloca,
                                          IRVisitor &visitor) {
  auto &builder = visitor.builder;

  auto elemType = elementType->codegen(visitor);
  for (int i = 0; i < size; i++) {
    auto index =
        llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(64, i));
    auto elemPtr =
        builder->CreateInBoundsGEP(elemType, alloca, index, "elemPtr");
    auto elemDefault = elementType->defaultInitialize(elemPtr, visitor);
  }

  return nullptr;
}