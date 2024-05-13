#pragma once

#include "ast/expr.hh"
#include "ast/sugar.hh"
#include <any>
#include <memory>

using std::any;
using std::unique_ptr;

class AstVisitor {
public:
  virtual ~AstVisitor() = default;

  // module
  virtual any visit(struct Module &module) = 0;

  // functions
  virtual any visit(struct FunctionDefinition &function) = 0;
  virtual any visit(struct Parameter &parameter) = 0;

  // statements
  virtual any visit(struct Block &block) = 0;
  virtual any visit(struct Return &ret) = 0;
  virtual any visit(struct IfElse &ifElse) = 0;
  virtual any visit(struct WhileStatement &whileStmt) = 0;
  virtual any visit(struct ExpressionStatement &exprStmt) = 0;
  virtual any visit(struct VariableDeclaration &varDecl) = 0;

  // expressions
  virtual any visit(struct Cast &cast) = 0;
  virtual any visit(struct IntLiteral &intLit) = 0;
  virtual any visit(struct FloatLiteral &floatLit) = 0;
  virtual any visit(struct BoolLiteral &boolLit) = 0;
  virtual any visit(struct StringLiteral &stringLit) = 0;
  virtual any visit(struct VariableReference &var) = 0;
  virtual any visit(struct ArrayReference &arrayRef) = 0;
  virtual any visit(struct FunctionCall &funcCall) = 0;
  virtual any visit(struct UnaryPrefixOp &unaryOp) = 0;
  virtual any visit(struct BinaryArithmetic &binaryOp) = 0;
  virtual any visit(struct BinaryComparison &binaryOp) = 0;
  virtual any visit(struct BinaryLogical &binaryOp) = 0;
  virtual any visit(struct TernaryExpr &ternaryOp) = 0;
  virtual any visit(struct Assignment &assignment) = 0;
  virtual any visit(struct ArrayAssignment &arrayAssignment) = 0;

  // sugar
  virtual any visit(struct sugar::ElifStatement &elifStmt) = 0;
  virtual any visit(struct sugar::IfElifElseStatement &elifElseStmt) = 0;
  virtual any visit(struct sugar::ForStatement &forStmt) = 0;
  virtual any visit(struct sugar::InIntervalExpr &inIntervalExpr) = 0;
};