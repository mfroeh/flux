#include "ast/type.hh"
#include "codegen/ir_visitor.hh"

using namespace std;

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

bool ClassType::canImplicitlyConvertTo(shared_ptr<Type> other) {
  return other == ClassType::get(name);
}

llvm::Value *ClassType::castTo(llvm::Value *value, shared_ptr<Type> to,
                               IRVisitor &visitor) {
  assert(false);
}

bool ClassType::canDefaultInitialize() const {
  for (auto &[name, field] : fields) {
    if (!field->type->canDefaultInitialize())
      return false;
  }
  return true;
}

llvm::Value *ClassType::defaultInitialize(llvm::Value *alloca,
                                          IRVisitor &visitor) {
  assert(canDefaultInitialize());

  auto type = codegen(visitor);
  int fieldCount = type->getStructNumElements();

  for (auto &[name, field] : fields) {
    auto fieldPtr = getFieldPtr(alloca, field->name, visitor);
    field->type->defaultInitialize(fieldPtr, visitor);
  }
  return alloca;
}

llvm::Value *ClassType::getFieldPtr(llvm::Value *alloca, string name,
                                    IRVisitor &visitor) {
  int index = fieldIndexMap.at(name);
  auto field = fields.at(name);
  auto fieldType = field->type->codegen(visitor);
  auto zero = llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(32, 0));
  auto llvmIndex =
      llvm::ConstantInt::get(*visitor.llvmContext, llvm::APInt(32, index));
  return visitor.builder->CreateInBoundsGEP(codegen(visitor), alloca,
                                            {zero, llvmIndex}, name + "Ptr");
}

llvm::Value *ClassType::getFieldValue(llvm::Value *alloca, string name,
                                      IRVisitor &visitor) {
  auto fieldPtr = getFieldPtr(alloca, name, visitor);
  auto fieldType = fields.at(name)->type->codegen(visitor);
  return visitor.builder->CreateLoad(fieldType, fieldPtr, name);
}

void ClassType::addField(shared_ptr<VariableSymbol> field) {
  assert(!fields.contains(field->name));
  fields[field->name] = field;
  fieldIndexMap[field->name] = fields.size() - 1;
}

shared_ptr<Type> ClassType::getFieldType(string name) {
  auto field = getField(name);
  return field ? field->type : nullptr;
}

shared_ptr<VariableSymbol> ClassType::getField(string name) {
  // may return nullptr
  return fields[name];
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