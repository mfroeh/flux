#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>

using std::shared_ptr;
using std::unique_ptr;

struct CodegenContext {
  CodegenContext();

  shared_ptr<llvm::LLVMContext> context;
  shared_ptr<llvm::Module> module;
  shared_ptr<llvm::IRBuilder<>> builder;

  shared_ptr<llvm::TargetMachine> targetMachine;
};