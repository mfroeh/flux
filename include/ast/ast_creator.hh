#pragma once

#include "FluxParser.h"
#include "ast/ast.hh"
#include "ast/class.hh"
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

  // classes
  ClassDefinition visitClassDefinition(FP::ClassDefinitionContext *ctx);
  FieldDeclaration visitFieldDeclaration(FP::FieldDeclarationContext *ctx);

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
  shared_ptr<Expr> visitPrefixUnary(FP::PrefixUnaryContext *ctx);
  shared_ptr<sugar::InIntervalExpr> visitInInterval(FP::InIntervalContext *ctx);
  shared_ptr<ArrayRef> visitArrayRef(FP::ArrayRefContext *ctx);
  shared_ptr<VarRef> visitVarRef(FP::VarRefContext *ctx);
  shared_ptr<FieldRef> visitFieldRef(FP::FieldRefContext *ctx);
  shared_ptr<BinaryComparison> visitBinaryComp(FP::BinaryCompContext *ctx);
  shared_ptr<BinaryLogical> visitBinaryLogical(FP::BinaryLogicalContext *ctx);
  shared_ptr<BinaryArithmetic>
  visitBinaryArithmetic(FP::BinaryArithmeticContext *ctx);
  shared_ptr<sugar::CompoundAssignment>
  visitCompoundAssignment(FP::CompoundAssignmentContext *ctx);
  shared_ptr<FunctionCall> visitFunctionCall(FP::FunctionCallContext *ctx);
  shared_ptr<MethodCall> visitMethodCall(FP::MethodCallContext *ctx);
  shared_ptr<Assignment> visitAssignment(FP::AssignmentContext *ctx);
  shared_ptr<Expr> visitParenExpr(FP::ParenExprContext *ctx);
  shared_ptr<TernaryExpr> visitTernary(FP::TernaryContext *ctx);
  shared_ptr<Expr> visitLiteralExpr(FP::LiteralExprContext *ctx);
  shared_ptr<Expr> visitArrayLiteral(FP::ArrayLiteralContext *ctx);
  shared_ptr<Expr> visitStructLiteral(FP::StructLiteralContext *ctx);
  shared_ptr<Expr> visitMalloc(FP::MallocContext *ctx);

  // misc
  vector<shared_ptr<Expr>> visitExpressionList(FP::ExpressionListContext *ctx);
  shared_ptr<Expr> visitLiteral(FP::LiteralContext *ctx);

  // types
  shared_ptr<Type> visitType(FP::TypeContext *ctx);
  shared_ptr<PointerType> visitPointerType(FP::PointerTypeContext *ctx);
  shared_ptr<ArrayType> visitArrayType(FP::ArrayTypeContext *ctx);
  shared_ptr<Type> visitScalarType(FP::ScalarTypeContext *ctx);
  shared_ptr<Type> visitBuiltinType(FP::BuiltinTypeContext *ctx);

  Interval visitInterval(FP::IntervalContext *ctx);
};