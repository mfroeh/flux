#include "ast/module.hh"
#include "codegen/ir_visitor.hh"
#include <memory>

using namespace std;
using namespace llvm;

IRVisitor::IRVisitor(ModuleContext &context, SymbolTable &symTab,
                     unique_ptr<llvm::LLVMContext> llvmContext)
    : context(context), symTab(symTab), llvmContext(std::move(llvmContext)),
      llvmModule(std::make_unique<llvm::Module>("main", *this->llvmContext)),
      builder(std::make_unique<llvm::IRBuilder<>>(*this->llvmContext)) {}

unique_ptr<llvm::Module> IRVisitor::visit(::Module &module) {
  for (auto &function : module.functions) {
    function.codegen(*this);
  }
  return std::move(llvmModule);
}