#pragma once

#include "ast/expr.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include <any>
#include <memory>

using std::any;
using std::shared_ptr;

class AbstractAstVisitor {
public:
  AbstractAstVisitor(ModuleContext &context) : context(context) {}
  virtual ~AbstractAstVisitor() = default;

  // module
  virtual any visit(struct Module &module) = 0;

  // classes
  virtual any visit(struct ClassDefinition &classDef) = 0;
  virtual any visit(struct FieldDeclaration &fieldDecl) = 0;

  // functions
  virtual any visit(struct FunctionDefinition &function) = 0;
  virtual any visit(struct Parameter &parameter) = 0;

  // statements
  virtual any visit(struct Block &block) = 0;
  virtual any visit(struct Return &ret) = 0;
  virtual any visit(struct IfElse &ifElse) = 0;
  virtual any visit(struct While &whileStmt) = 0;
  virtual any visit(struct ExpressionStatement &exprStmt) = 0;
  virtual any visit(struct VariableDeclaration &varDecl) = 0;
  virtual any visit(struct StandaloneBlock &standaloneBlock) = 0;
  virtual any visit(struct Print &print) = 0;

  // expressions
  virtual any visit(struct Cast &cast) = 0;
  virtual any visit(struct IntLiteral &intLit) = 0;
  virtual any visit(struct FloatLiteral &floatLit) = 0;
  virtual any visit(struct BoolLiteral &boolLit) = 0;
  virtual any visit(struct StringLiteral &stringLit) = 0;
  virtual any visit(struct ArrayLiteral &arrInit) = 0;
  virtual any visit(struct StructLiteral &structLit) = 0;

  virtual any visit(struct VarRef &var) = 0;
  virtual any visit(struct FieldRef &fieldRef) = 0;
  virtual any visit(struct ArrayRef &arr) = 0;
  virtual any visit(struct FunctionCall &funcCall) = 0;
  virtual any visit(struct MethodCall &methodCall) = 0;
  virtual any visit(struct UnaryPrefixOp &unaryOp) = 0;
  virtual any visit(struct BinaryArithmetic &binaryOp) = 0;
  virtual any visit(struct BinaryComparison &binaryOp) = 0;
  virtual any visit(struct BinaryLogical &binaryOp) = 0;
  virtual any visit(struct TernaryExpr &ternaryOp) = 0;
  virtual any visit(struct Assignment &assignment) = 0;
  virtual any visit(struct Pointer &pointer) = 0;
  virtual any visit(struct Dereference &dereference) = 0;
  virtual any visit(struct Halloc &halloc) = 0;

  // sugar
  virtual any visit(struct sugar::ElifStatement &elifStmt) = 0;
  virtual any visit(struct sugar::IfElifElseStatement &elifElseStmt) = 0;
  virtual any visit(struct sugar::ForLoop &forStmt) = 0;
  virtual any visit(struct sugar::InIntervalExpr &inIntervalExpr) = 0;
  virtual any visit(struct sugar::CompoundAssignment &compoundAssignment) = 0;

protected:
  ModuleContext &context;
};

class AstVisitor : public AbstractAstVisitor {
public:
  AstVisitor(ModuleContext &context, SymbolTable &symTab)
      : AbstractAstVisitor(context), symTab(symTab) {}

  // module
  any visit(struct Module &module) override;

  // classes
  any visit(struct ClassDefinition &classDef) override;
  any visit(struct FieldDeclaration &fieldDecl) override;

  // functions
  any visit(struct FunctionDefinition &function) override;
  any visit(struct Parameter &parameter) override;

  // statements
  any visit(struct Block &block) override;
  any visit(struct Return &ret) override;
  any visit(struct IfElse &ifElse) override;
  any visit(struct While &whileStmt) override;
  any visit(struct ExpressionStatement &exprStmt) override;
  any visit(struct VariableDeclaration &varDecl) override;
  any visit(struct StandaloneBlock &standaloneBlock) override;
  any visit(struct Print &print) override;

  // expressions
  any visit(struct Cast &cast) override;
  any visit(struct IntLiteral &intLit) override;
  any visit(struct FloatLiteral &floatLit) override;
  any visit(struct BoolLiteral &boolLit) override;
  any visit(struct StringLiteral &stringLit) override;
  any visit(struct ArrayLiteral &arrInit) override;
  any visit(struct StructLiteral &structLit) override;

  any visit(struct VarRef &var) override;
  any visit(struct FieldRef &fieldRef) override;
  any visit(struct ArrayRef &arr) override;
  any visit(struct FunctionCall &funcCall) override;
  any visit(struct MethodCall &methodCall) override;
  any visit(struct UnaryPrefixOp &unaryOp) override;
  any visit(struct BinaryArithmetic &binaryOp) override;
  any visit(struct BinaryComparison &binaryOp) override;
  any visit(struct BinaryLogical &binaryOp) override;
  any visit(struct TernaryExpr &ternaryOp) override;
  any visit(struct Assignment &assignment) override;
  any visit(struct Pointer &pointer) override;
  any visit(struct Dereference &dereference) override;
  any visit(struct Halloc &halloc) override;

  // sugar
  any visit(struct sugar::ElifStatement &elifStmt) override;
  any visit(struct sugar::IfElifElseStatement &elifElseStmt) override;
  any visit(struct sugar::ForLoop &forStmt) override;
  any visit(struct sugar::InIntervalExpr &inIntervalExpr) override;
  any visit(struct sugar::CompoundAssignment &compoundAssignment) override;

protected:
  SymbolTable &symTab;
};