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
}