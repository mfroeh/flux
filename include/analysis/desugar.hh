#pragma once

#include "ast/sugar.hh"
#include "module_context.hh"
#include "visitor.hh"
#include <memory>
using std::shared_ptr;

class Desugarer : public AbstractAstVisitor {
public:
  Desugarer(ModuleContext &context) : AbstractAstVisitor(context) {}

  // module
  any visit(struct Module &module) override;

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
  any visit(struct VariableReference &var) override;
  any visit(struct ArrayReference &arr) override;
  any visit(struct FunctionCall &funcCall) override;
  any visit(struct UnaryPrefixOp &unaryOp) override;
  any visit(struct BinaryArithmetic &binaryOp) override;
  any visit(struct BinaryComparison &binaryOp) override;
  any visit(struct BinaryLogical &binaryOp) override;
  any visit(struct TernaryExpr &ternaryOp) override;
  any visit(struct Assignment &assignment) override;

  // sugar
  any visit(struct sugar::ElifStatement &elifStmt) override;
  any visit(struct sugar::IfElifElseStatement &elifElseStmt) override;
  any visit(struct sugar::ForLoop &forStmt) override;
  any visit(struct sugar::InIntervalExpr &inIntervalExpr) override;
  any visit(struct sugar::CompoundAssignment &compoundAssignment) override;
};