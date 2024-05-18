#pragma once

#include "ast/expr.hh"
#include "codegen/context.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <memory>

using std::unique_ptr;

class IRVisitor {
public:
  IRVisitor(ModuleContext &context, SymbolTable &symTab,
            CodegenContext &codegenContext);
  ~IRVisitor() = default;

  // module
  shared_ptr<llvm::Module> visit(struct Module &module);

  // functions (might change to return llvm::Function*)
  void visit(struct FunctionDefinition &function);

  // statements
  void visit(struct Block &block);
  void visit(struct StandaloneBlock &standaloneBlock);
  void visit(struct Return &ret);
  void visit(struct IfElse &ifElse);
  void visit(struct While &whileStmt);
  void visit(struct ExpressionStatement &exprStmt);
  void visit(struct VariableDeclaration &varDecl);

  // expressions
  llvm::Value *visit(struct Cast &cast);
  llvm::Value *visit(struct IntLiteral &literal);
  llvm::Value *visit(struct FloatLiteral &literal);
  llvm::Value *visit(struct BoolLiteral &literal);
  llvm::Value *visit(struct StringLiteral &literal);
  llvm::Value *visit(struct ArrayLiteral &literal);

  llvm::Value *visit(struct VariableReference &var);
  llvm::Value *visit(struct ArrayReference &array);
  llvm::Value *visit(struct FunctionCall &call);
  llvm::Value *visit(struct UnaryPrefixOp &operation);
  llvm::Value *visit(struct BinaryArithmetic &operation);
  llvm::Value *visit(struct BinaryComparison &operation);
  llvm::Value *visit(struct BinaryLogical &operation);
  llvm::Value *visit(struct TernaryExpr &operation);
  llvm::Value *visit(struct Assignment &assignment);
  llvm::Value *visit(struct Pointer &pointer);
  llvm::Value *visit(struct Dereference &dereference);

public:
  ModuleContext &context;
  SymbolTable &symTab;
  CodegenContext &codegenContext;

  // llvm
  shared_ptr<llvm::LLVMContext> &llvmContext;
  shared_ptr<llvm::Module> &llvmModule;
  shared_ptr<llvm::IRBuilder<>> &builder;
};