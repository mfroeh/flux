#include "ast/class.hh"
#include "codegen/ir_visitor.hh"

using namespace llvm;
using namespace std;

void IRVisitor::visit(ClassDefinition &classDef) {
  cout << "codegen class " << classDef.name << endl;
  vector<llvm::Type *> fieldTypes;
  for (auto &field : classDef.fields) {
    fieldTypes.push_back(field.type->codegen(*this));
  }
  auto classType = StructType::create(*llvmContext, fieldTypes, classDef.name);
  auto symbol = symTab.lookupClass(classDef.name);
  assert(symbol);
  symbol->llvmType = classType;

  for (auto &method : classDef.methods) {
    // TODO: remove only dbg
    auto symbol = symTab.lookupFunction(method.mangledName);
    assert(symbol->parameters.size() >= 1);
    assert(symbol->parameters.size() == method.parameters.size());

    method.codegen(*this);
  }
}