#include "ast/module.hh"
#include "codegen/context.hh"
#include "codegen/ir_visitor.hh"
#include <llvm/IR/Module.h>
#include <memory>

using namespace std;
using namespace llvm;

IRVisitor::IRVisitor(ModuleContext &context, SymbolTable &symTab,
                     CodegenContext &codegenContext)
    : context(context), symTab(symTab), codegenContext(codegenContext),
      llvmContext(codegenContext.context), builder(codegenContext.builder),
      llvmModule(codegenContext.module) {}

shared_ptr<llvm::Module> IRVisitor::visit(::Module &module) {
  cout << "codegen module" << endl;
  for (auto &classDef : module.classes) {
    classDef.codegen(*this);
  }

  for (auto &function : module.functions) {
    function.codegen(*this);
  }
  return llvmModule;
}