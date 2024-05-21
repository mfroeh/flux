#pragma once

#include "ast/class.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/sugar.hh"
#include "module_context.hh"
#include "symbol_table.hh"
#include "visitor.hh"
#include <memory>
using std::shared_ptr;

class Desugarer : public AbstractAstVisitor {
public:
  Desugarer(ModuleContext &context) : AbstractAstVisitor(context) {}

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
};

class NonTypedDesugarer : public Desugarer {
public:
  NonTypedDesugarer(ModuleContext &context) : Desugarer(context) {}

  // sugar
  any visit(struct sugar::ElifStatement &elifStmt) override;
  any visit(struct sugar::IfElifElseStatement &elifElseStmt) override;
  any visit(struct sugar::ForLoop &forStmt) override;
  any visit(struct sugar::InIntervalExpr &inIntervalExpr) override;
  any visit(struct sugar::CompoundAssignment &compoundAssignment) override;

  any visit(struct FunctionDefinition &method) override;
};

class TypedDesugarer : public Desugarer {
public:
  TypedDesugarer(ModuleContext &context, SymbolTable &symTab)
      : Desugarer(context), symTab(symTab) {}

  any visit(struct MethodCall &methodCall) override;

private:
  SymbolTable &symTab;
};