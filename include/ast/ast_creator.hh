#pragma once

#include "FluxParserVisitor.h"

using std::any;

class AstCreator : public FluxParserVisitor {
  // module
  any visitModule(FluxParser::ModuleContext *ctx) override;

  // classes
  any visitClassDefinition(FluxParser::ClassDefinitionContext *ctx) override;
  any visitFieldDeclaration(FluxParser::FieldDeclarationContext *ctx) override;

  // functions
  any visitBlockFunction(FluxParser::BlockFunctionContext *ctx) override;
  any visitLambdaFunction(FluxParser::LambdaFunctionContext *ctx) override;
  any visitParameterList(FluxParser::ParameterListContext *ctx) override;
  any visitParameter(FluxParser::ParameterContext *ctx) override;

  // statements
  any visitBlock(FluxParser::BlockContext *ctx) override;
  any visitStatement(FluxParser::StatementContext *ctx) override;
  any visitExpressionStatement(
      FluxParser::ExpressionStatementContext *ctx) override;
  any visitVariableDeclaration(
      FluxParser::VariableDeclarationContext *ctx) override;
  any visitReturnStatement(FluxParser::ReturnStatementContext *ctx) override;
  any visitWhileLoop(FluxParser::WhileLoopContext *ctx) override;
  any visitForLoop(FluxParser::ForLoopContext *ctx) override;
  any visitIfStatement(FluxParser::IfStatementContext *ctx) override;
  any visitElseIfStatement(FluxParser::ElseIfStatementContext *ctx) override;
  any visitElseStatement(FluxParser::ElseStatementContext *ctx) override;

  // expressions
  any visitPrefixUnaryExpr(FluxParser::PrefixUnaryExprContext *ctx) override;
  any visitInIntervalExpr(FluxParser::InIntervalExprContext *ctx) override;
  any visitArrayRefExpr(FluxParser::ArrayRefExprContext *ctx) override;
  any visitIdentifierExpr(FluxParser::IdentifierExprContext *ctx) override;
  any visitBinaryExpr(FluxParser::BinaryExprContext *ctx) override;
  any visitCompoundAssignmentExpr(
      FluxParser::CompoundAssignmentExprContext *ctx) override;
  any visitLiteralExpr(FluxParser::LiteralExprContext *ctx) override;
  any visitCallExpr(FluxParser::CallExprContext *ctx) override;
  any visitAssignmentExpr(FluxParser::AssignmentExprContext *ctx) override;
  any visitParenExpr(FluxParser::ParenExprContext *ctx) override;
  any visitTernaryExpr(FluxParser::TernaryExprContext *ctx) override;
  any visitLiteral(FluxParser::LiteralContext *ctx) override;

  // misc
  any visitExpressionList(FluxParser::ExpressionListContext *ctx) override;
  any visitType(FluxParser::TypeContext *ctx) override;
  any visitArrayType(FluxParser::ArrayTypeContext *ctx) override;
  any visitBuiltinType(FluxParser::BuiltinTypeContext *ctx) override;
  any visitInterval(FluxParser::IntervalContext *ctx) override;
};