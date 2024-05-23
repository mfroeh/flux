#include "codegen/optim.hh"
#include "codegen/context.hh"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Scalar/DeadStoreElimination.h>

using namespace llvm;
using namespace std;

IROptimizer::IROptimizer(ModuleContext &moduleContext,
                         CodegenContext &codegenContext)
    : moduleContext(moduleContext), module(codegenContext.module),
      ctx(codegenContext) {}

void IROptimizer::optimize() {
  auto fpm = make_unique<FunctionPassManager>();
  auto lam = make_unique<LoopAnalysisManager>();
  auto fam = make_unique<FunctionAnalysisManager>();
  auto cgsc = make_unique<CGSCCAnalysisManager>();
  auto mam = make_unique<ModuleAnalysisManager>();
  auto pic = make_unique<PassInstrumentationCallbacks>();
  auto si = make_unique<StandardInstrumentations>(*ctx.context, true);

  fpm->addPass(InstCombinePass());
  fpm->addPass(ReassociatePass());
  fpm->addPass(GVNPass());
  fpm->addPass(SimplifyCFGPass());
  fpm->addPass(DSEPass());

  PassBuilder pb;
  pb.registerModuleAnalyses(*mam);
  pb.registerCGSCCAnalyses(*cgsc);
  pb.registerFunctionAnalyses(*fam);
  pb.registerLoopAnalyses(*lam);
  pb.crossRegisterProxies(*lam, *fam, *cgsc, *mam);

  for (auto &function : module->functions()) {
    bool errors = verifyFunction(function);
    if (errors) {
      function.print(errs());
      exit(1);
    }
    // todo: segfaults
    fpm->run(function, *fam);
  }
}