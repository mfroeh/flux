#include "codegen/context.hh"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

using namespace llvm;
using namespace std;

void initializeTargets() {
  // initialize the target registry etc.
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  // interpreter
  // LLVMLinkInMCJIT();
}

CodegenContext::CodegenContext() {
  context = std::make_shared<LLVMContext>();
  module = std::make_shared<Module>("mainModule", *context);
  builder = std::make_shared<IRBuilder<>>(*context);

  // intialize LLVM for object code gen
  initializeTargets();

  auto targetTriple = llvm::sys::getDefaultTargetTriple();
  string err;
  auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    errs() << err;
    exit(1);
  }

  string cpu = "generic";
  string features = "";

  TargetOptions opt;
  targetMachine = shared_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, cpu, features, opt, Reloc::PIC_));

  module->setDataLayout(targetMachine->createDataLayout());
  module->setTargetTriple(targetTriple);
}