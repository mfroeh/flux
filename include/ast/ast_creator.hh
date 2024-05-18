#pragma once

#include "FluxParser.h"
#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"
#include <vector>

using std::any;
using std::shared_ptr;

struct Interval {
  shared_ptr<Expr> lower;
  shared_ptr<Expr> upper;
  sugar::InIntervalExpr::IntervalKind kind;
};

class AstCreator {
  using FP = FluxParser;

public:
  // module
  Module visitModule(FP::ModuleContext *ctx);

  // functions
  FunctionDefinition
  visitFunctionDefinition(FP::FunctionDefinitionContext *ctx);
  vector<Parameter> visitParameterList(FP::ParameterListContext *ctx);
  Parameter visitParameter(FP::ParameterContext *ctx);

  // statements
  Block visitBlock(FP::BlockContext *ctx);
  Block visitSingleLineBody(FP::StatementContext *ctx);
  shared_ptr<Statement> visitStatement(FP::StatementContext *ctx);
  shared_ptr<ExpressionStatement>
  visitExpressionStatement(FP::ExpressionStatementContext *ctx);
  shared_ptr<VariableDeclaration>
  visitVariableDeclaration(FP::VariableDeclarationContext *ctx);
  shared_ptr<Return> visitReturnStatement(FP::ReturnStatementContext *ctx);
  shared_ptr<While> visitWhileLoop(FP::WhileLoopContext *ctx);
  shared_ptr<StandaloneBlock>
  visitStandaloneBlock(FP::StandaloneBlockContext *ctx);
  shared_ptr<sugar::ForLoop> visitForLoop(FP::ForLoopContext *ctx);
  shared_ptr<sugar::IfElifElseStatement>
  visitIfStatement(FP::IfStatementContext *ctx);
  shared_ptr<sugar::ElifStatement>
  visitElseIfStatement(FP::ElseIfStatementContext *ctx);
  Block visitElseBlock(FP::ElseBlockContext *ctx);

  // expressions
  shared_ptr<Expr> visitExpression(FP::ExpressionContext *ctx);
  shared_ptr<Expr> visitPrefixUnaryExpr(FP::PrefixUnaryExprContext *ctx);
  shared_ptr<sugar::InIntervalExpr>
  visitInIntervalExpr(FP::InIntervalExprContext *ctx);
  shared_ptr<ArrayReference> visitArrayRefExpr(FP::ArrayRefExprContext *ctx);
  shared_ptr<VariableReference>
  visitIdentifierExpr(FP::IdentifierExprContext *ctx);
  shared_ptr<BinaryComparison>
  visitBinaryCompExpr(FP::BinaryCompExprContext *ctx);
  shared_ptr<BinaryLogical>
  visitBinaryLogicalExpr(FP::BinaryLogicalExprContext *ctx);
  shared_ptr<BinaryArithmetic>
  visitBinaryArithmeticExpr(FP::BinaryArithmeticExprContext *ctx);
  shared_ptr<sugar::CompoundAssignment>
  visitCompoundAssignmentExpr(FP::CompoundAssignmentExprContext *ctx);
  shared_ptr<Expr> visitLiteralExpr(FP::LiteralExprContext *ctx);
  shared_ptr<FunctionCall> visitCallExpr(FP::CallExprContext *ctx);
  shared_ptr<Assignment> visitAssignmentExpr(FP::AssignmentExprContext *ctx);
  shared_ptr<Expr> visitParenExpr(FP::ParenExprContext *ctx);
  shared_ptr<TernaryExpr> visitTernaryExpr(FP::TernaryExprContext *ctx);
  shared_ptr<Expr> visitLiteral(FP::LiteralContext *ctx);
  shared_ptr<Expr> visitArrayLiteral(FP::ArrayLiteralContext *ctx);

  // misc
  vector<shared_ptr<Expr>> visitExpressionList(FP::ExpressionListContext *ctx);
  shared_ptr<Type> visitType(FP::TypeContext *ctx);
  shared_ptr<PointerType> visitPointerType(FP::PointerTypeContext *ctx);
  shared_ptr<Type> visitNonPointerType(FP::NonPointerTypeContext *ctx);
  shared_ptr<ArrayType> visitArrayType(FP::ArrayTypeContext *ctx);
  shared_ptr<Type> visitBuiltinType(FP::BuiltinTypeContext *ctx);

  Interval visitInterval(FP::IntervalContext *ctx);

  // classes
  any visitClassDefinition(FP::ClassDefinitionContext *ctx);
  any visitFieldDeclaration(FP::FieldDeclarationContext *ctx);
};