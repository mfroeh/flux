#pragma once

#include "symbol_table.hh"
#include "visitor.hh"

class TypeChecker : public AstVisitor {
public:
  TypeChecker(ModuleContext &context, SymbolTable &symTab)
      : AstVisitor(context, symTab) {}

  // functions
  any visit(struct FunctionDefinition &function) override;
  any visit(struct Parameter &param) override;

  // statements
  any visit(struct Return &ret) override;
  any visit(struct IfElse &ifElse) override;
  any visit(struct While &whileStmt) override;
  any visit(struct ExpressionStatement &exprStmt) override;
  any visit(struct VariableDeclaration &varDecl) override;

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
  any visit(struct Assignment &assignment) override;
  any visit(struct TernaryExpr &ternaryOp) override;
};