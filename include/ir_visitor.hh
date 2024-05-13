#pragma once

#include "ast/expr.hh"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

class IRVisitor {
public:
  virtual ~IRVisitor() = default;

  // module
  virtual void visit(struct Module &module) = 0;

  // functions
  virtual llvm::Function *visit(struct FunctionDefinition &function) = 0;

  // statements
  virtual void visit(struct Block &block) = 0;
  virtual void visit(struct Return &ret) = 0;
  virtual void visit(struct IfElse &ifElse) = 0;
  virtual void visit(struct WhileStatement &whileStmt) = 0;
  virtual void visit(struct ExpressionStatement &exprStmt) = 0;
  virtual void visit(struct VariableDeclaration &varDecl) = 0;

  // expressions
  virtual llvm::Value *visit(struct Cast &cast) = 0;
  virtual llvm::Value *visit(struct IntLiteral &literal) = 0;
  virtual llvm::Value *visit(struct FloatLiteral &literal) = 0;
  virtual llvm::Value *visit(struct BoolLiteral &literal) = 0;
  virtual llvm::Value *visit(struct StringLiteral &literal) = 0;

  virtual llvm::Value *visit(struct VariableReference &reference) = 0;
  virtual llvm::Value *visit(struct ArrayReference &reference) = 0;
  virtual llvm::Value *visit(struct FunctionCall &call) = 0;
  virtual llvm::Value *visit(struct UnaryPrefixOp &operation) = 0;
  virtual llvm::Value *visit(struct BinaryArithmetic &operation) = 0;
  virtual llvm::Value *visit(struct BinaryComparison &operation) = 0;
  virtual llvm::Value *visit(struct BinaryLogical &operation) = 0;
  virtual llvm::Value *visit(struct TernaryExpr &operation) = 0;
  virtual llvm::Value *visit(struct Assignment &assignment) = 0;
  virtual llvm::Value *visit(struct ArrayAssignment &assignment) = 0;

  llvm::LLVMContext context;
};