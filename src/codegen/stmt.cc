#include "ast/stmt.hh"
#include "ast/type.hh"
#include "codegen/ir_visitor.hh"
#include <iostream>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

using namespace llvm;
using namespace std;

void IRVisitor::visit(Block &block) {
  for (auto &stmt : block.statements) {
    stmt->codegen(*this);
  }
}

void IRVisitor::visit(StandaloneBlock &block) { visit(block.block); }

void IRVisitor::visit(ExpressionStatement &stmt) { stmt.expr->codegen(*this); }

void IRVisitor::visit(VariableDeclaration &stmt) {
  auto symbol = symTab.lookupVariable(stmt.mangledName);
  assert(symbol);

  auto llvmType = symbol->type->codegen(*this);

  auto alloca = builder->CreateAlloca(llvmType, nullptr, stmt.name);
  symbol->alloc = alloca;

  if (!stmt.initializer) {
    symbol->type->defaultInitialize(alloca, *this);
  } else {
    auto value = stmt.initializer->codegen(*this);
    builder->CreateStore(value, alloca);
  }
}

void IRVisitor::visit(Return &stmt) {
  if (stmt.expression) {
    auto value = stmt.expression->codegen(*this);
    builder->CreateRet(value);
  } else {
    builder->CreateRetVoid();
  }
}

void IRVisitor::visit(IfElse &stmt) {
  auto condition = stmt.condition->codegen(*this);
  auto function = builder->GetInsertBlock()->getParent();

  auto thenBlock = BasicBlock::Create(*llvmContext, "if.then", function);
  auto elseBlock = BasicBlock::Create(*llvmContext, "if.else", function);
  auto mergeBlock = BasicBlock::Create(*llvmContext, "if.exit", function);

  builder->CreateCondBr(condition, thenBlock, elseBlock);

  // then block
  builder->SetInsertPoint(thenBlock);
  stmt.thenBlock.codegen(*this);

  // if we didn't see a return
  if (!builder->GetInsertBlock()->getTerminator())
    builder->CreateBr(mergeBlock);
  thenBlock = builder->GetInsertBlock();

  // else block
  builder->SetInsertPoint(elseBlock);
  stmt.elseBlock.codegen(*this);
  if (!builder->GetInsertBlock()->getTerminator())
    builder->CreateBr(mergeBlock);
  elseBlock = builder->GetInsertBlock();

  // merge block
  builder->SetInsertPoint(mergeBlock);
}

void IRVisitor::visit(While &stmt) {
  auto function = builder->GetInsertBlock()->getParent();

  auto conditionBlock =
      BasicBlock::Create(*llvmContext, "while.cond", function);
  auto bodyBlock = BasicBlock::Create(*llvmContext, "while.body", function);
  auto afterBlock = BasicBlock::Create(*llvmContext, "while.after", function);

  builder->CreateBr(conditionBlock);

  // condition block
  builder->SetInsertPoint(conditionBlock);
  auto condition = stmt.condition->codegen(*this);
  builder->CreateCondBr(condition, bodyBlock, afterBlock);

  // body block
  builder->SetInsertPoint(bodyBlock);
  stmt.body.codegen(*this);
  builder->CreateBr(conditionBlock);

  // after block
  builder->SetInsertPoint(afterBlock);
}