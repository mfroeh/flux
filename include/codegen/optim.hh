#pragma once

#include "codegen/context.hh"
#include "module_context.hh"
#include <llvm/IR/Module.h>
class IROptimizer {
public:
  IROptimizer(ModuleContext &moduleContext, CodegenContext &ctx);

  void optimize();

protected:
  CodegenContext &ctx;
  ModuleContext &moduleContext;
  shared_ptr<llvm::Module> module;
};