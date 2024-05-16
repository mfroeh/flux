#include "ast/function.hh"
#include "codegen/ir_visitor.hh"
#include <llvm-c/Error.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace std;

void IRVisitor::visit(FunctionDefinition &function) {
  // for inline functions
  auto previousBB = builder->GetInsertBlock();

  vector<llvm::Type *> parameterTypes;
  for (auto &param : function.parameters) {
    parameterTypes.push_back(param.type->codegen(*this));
  }

  auto returnType = function.returnType->codegen(*this);
  auto functionType =
      llvm::FunctionType::get(returnType, parameterTypes, false);

  // create a new function and attach to module
  auto llvmFunction = Function::Create(functionType, Function::InternalLinkage,
                                       function.mangledName, *llvmModule);
  auto funcSymbol = symTab.lookupFunction(function.mangledName);
  funcSymbol->llvmFunction = llvmFunction;

  // create function entry block
  auto bb = BasicBlock::Create(*llvmContext, "alloca", llvmFunction);
  builder->SetInsertPoint(bb);
  auto body = BasicBlock::Create(*llvmContext, "entry", llvmFunction);

  // push all parameters to stack
  for (int i = 0; i < function.parameters.size(); i++) {
    auto &param = function.parameters[i];
    auto symbol = symTab.lookupVariable(param.mangledName);
    assert(symbol);

    auto arg = llvmFunction->arg_begin() + i;
    auto alloca =
        builder->CreateAlloca(param.type->codegen(*this), nullptr, param.name);
    // not sure why we need this
    builder->CreateStore(arg, alloca);

    symbol->alloc = alloca;
  }

  builder->CreateBr(body);
  builder->SetInsertPoint(body);

  // codegen function body
  function.body.codegen(*this);

  string err;
  auto errStream = raw_string_ostream(err);
  bool errors = llvm::verifyFunction(*llvmFunction, &errStream);
  if (errors) {
    errs() << "Verification failed: " << errStream.str();
    outs() << *llvmFunction;
    throw runtime_error("Function verification failed");
  }

  // for inline functions
  builder->SetInsertPoint(previousBB);
}