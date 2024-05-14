#include "ast/ast_creator.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"
#include <memory>
#include <ranges>

using namespace std;
using namespace ranges;

// module
Module AstCreator::visitModule(FluxParser::ModuleContext *ctx) {
  vector<FunctionDefinition> functions;
  ranges::copy(ctx->functionDefinition() | views::transform([this](auto &func) {
                 return visitFunctionDefinition(func);
               }),
               back_inserter(functions));
  return Module(Tokens(ctx), functions);
}

// functions
FunctionDefinition AstCreator::visitFunctionDefinition(
    FluxParser::FunctionDefinitionContext *ctx) {
  bool isLambda = ctx->expression() != nullptr;
  string name = ctx->Identifier()->getText();
  auto parameters = visitParameterList(ctx->parameterList());
  auto returnType = visitType(ctx->type());

  optional<Block> body;
  if (isLambda) {
    auto returnStmt = make_shared<Return>(Tokens(ctx->expression()),
                                          visitExpression(ctx->expression()));
    body = Block(returnStmt->tokens, vector<shared_ptr<Statement>>{returnStmt},
                 false);
  } else {
    body = Block(visitBlock(ctx->block()));
  }

  return FunctionDefinition(Tokens(ctx), name, parameters, returnType,
                            body.value());
}

vector<Parameter>
AstCreator::visitParameterList(FluxParser::ParameterListContext *ctx) {
  vector<Parameter> parameters;
  if (!ctx)
    return parameters;
  ranges::copy(ctx->parameter() | views::transform([this](auto &param) {
                 return visitParameter(param);
               }),
               back_inserter(parameters));
  return parameters;
}
Parameter AstCreator::visitParameter(FluxParser::ParameterContext *ctx) {
  string name = ctx->Identifier()->getText();
  auto type = visitType(ctx->type());
  return Parameter(Tokens(ctx), name, type);
}

// statements
Block AstCreator::visitBlock(FluxParser::BlockContext *ctx) {
  vector<shared_ptr<Statement>> statements;
  if (!ctx)
    return Block();
  ranges::copy(ctx->statement() | views::transform([this](auto &stmt) {
                 return visitStatement(stmt);
               }),
               back_inserter(statements));

  // TODO: so far we don't allow parsing standalone blocks, they are only
  // created during desguaring of for loops
  bool isStandalone = false;
  return Block(Tokens(ctx), statements, isStandalone);
}

Block AstCreator::visitSingleLineBody(FluxParser::StatementContext *ctx) {
  auto statement = visitStatement(ctx);
  return Block(statement->tokens, vector<shared_ptr<Statement>>{statement},
               false);
}

shared_ptr<Statement>
AstCreator::visitStatement(FluxParser::StatementContext *ctx) {
  if (auto exprStmt = ctx->expressionStatement())
    return visitExpressionStatement(exprStmt);
  else if (auto varDecl = ctx->variableDeclaration())
    return visitVariableDeclaration(varDecl);
  else if (auto returnStmt = ctx->returnStatement())
    return visitReturnStatement(returnStmt);
  else if (auto whileLoop = ctx->whileLoop())
    return visitWhileLoop(whileLoop);
  else if (auto forLoop = ctx->forLoop())
    return visitForLoop(forLoop);
  else if (auto ifStmt = ctx->ifStatement())
    return visitIfStatement(ifStmt);
  else
    throw runtime_error("Unknown statement type");
}

shared_ptr<ExpressionStatement> AstCreator::visitExpressionStatement(
    FluxParser::ExpressionStatementContext *ctx) {
  return make_shared<ExpressionStatement>(Tokens(ctx),
                                          visitExpression(ctx->expression()));
}

shared_ptr<VariableDeclaration> AstCreator::visitVariableDeclaration(
    FluxParser::VariableDeclarationContext *ctx) {
  string name = ctx->Identifier()->getText();
  auto type = visitType(ctx->type());
  shared_ptr<Expr> initializer;
  if (ctx->expression())
    initializer = visitExpression(ctx->expression());
  return make_shared<VariableDeclaration>(Tokens(ctx), name, type, initializer);
}

shared_ptr<Return>
AstCreator::visitReturnStatement(FluxParser::ReturnStatementContext *ctx) {
  return make_shared<Return>(Tokens(ctx), visitExpression(ctx->expression()));
}

shared_ptr<While>
AstCreator::visitWhileLoop(FluxParser::WhileLoopContext *ctx) {
  auto condition = visitExpression(ctx->expression());
  auto body = ctx->block() ? visitBlock(ctx->block())
                           : visitSingleLineBody(ctx->statement());

  return make_shared<While>(Tokens(ctx), condition, body);
}

shared_ptr<sugar::ForLoop>
AstCreator::visitForLoop(FluxParser::ForLoopContext *ctx) {
  auto init = visitStatement(ctx->statement(0)); // let i = 0;
  auto conditionStatement =
      visitExpressionStatement(ctx->expressionStatement()); // i < N;
  auto condition = conditionStatement->expression;
  auto post = visitStatement(ctx->statement(1)); // i += 1;

  auto body = ctx->block() ? visitBlock(ctx->block())
                           : visitSingleLineBody(ctx->statement(2));

  return make_shared<sugar::ForLoop>(Tokens(ctx), init, condition, post, body);
}

shared_ptr<sugar::IfElifElseStatement>
AstCreator::visitIfStatement(FluxParser::IfStatementContext *ctx) {
  auto condition = visitExpression(ctx->expression());
  auto thenBlock = ctx->block() ? visitBlock(ctx->block())
                                : visitSingleLineBody(ctx->statement());

  vector<shared_ptr<sugar::ElifStatement>> elifs;
  ranges::copy(ctx->elseIfStatement() | views::transform([this](auto &elif) {
                 return visitElseIfStatement(elif);
               }),
               back_inserter(elifs));

  optional<Block> elseBlock;
  if (ctx->elseBlock())
    elseBlock = visitElseBlock(ctx->elseBlock());

  return make_shared<sugar::IfElifElseStatement>(
      Tokens(ctx), condition, thenBlock, elifs, elseBlock.value_or(Block()));
}

shared_ptr<sugar::ElifStatement>
AstCreator::visitElseIfStatement(FluxParser::ElseIfStatementContext *ctx) {
  auto condition = visitExpression(ctx->expression());
  auto thenBlock = ctx->block() ? visitBlock(ctx->block())
                                : visitSingleLineBody(ctx->statement());
  return make_shared<sugar::ElifStatement>(Tokens(ctx), condition, thenBlock);
}

Block AstCreator::visitElseBlock(FluxParser::ElseBlockContext *ctx) {
  return visitBlock(ctx->block());
}

// expressions
shared_ptr<Expr>
AstCreator::visitExpression(FluxParser::ExpressionContext *ctx) {
  using FP = FluxParser;
  if (auto expr = dynamic_cast<FP::ParenExprContext *>(ctx))
    return visitParenExpr(expr);
  else if (auto expr = dynamic_cast<FP::LiteralExprContext *>(ctx))
    return visitLiteralExpr(expr);
  else if (auto expr = dynamic_cast<FP::IdentifierExprContext *>(ctx))
    return visitIdentifierExpr(expr);
  else if (auto expr = dynamic_cast<FP::ArrayRefExprContext *>(ctx))
    return visitArrayRefExpr(expr);
  else if (auto expr = dynamic_cast<FP::CallExprContext *>(ctx))
    return visitCallExpr(expr);
  else if (auto expr = dynamic_cast<FP::PrefixUnaryExprContext *>(ctx))
    return visitPrefixUnaryExpr(expr);
  else if (auto expr = dynamic_cast<FP::BinaryLogicalExprContext *>(ctx))
    return visitBinaryLogicalExpr(expr);
  else if (auto expr = dynamic_cast<FP::BinaryArithmeticExprContext *>(ctx))
    return visitBinaryArithmeticExpr(expr);
  else if (auto expr = dynamic_cast<FP::BinaryCompExprContext *>(ctx))
    return visitBinaryCompExpr(expr);
  else if (auto expr = dynamic_cast<FP::TernaryExprContext *>(ctx))
    return visitTernaryExpr(expr);
  else if (auto expr = dynamic_cast<FP::InIntervalExprContext *>(ctx))
    return visitInIntervalExpr(expr);
  else if (auto expr = dynamic_cast<FP::CompoundAssignmentExprContext *>(ctx))
    return visitCompoundAssignmentExpr(expr);
  else if (auto expr = dynamic_cast<FP::AssignmentExprContext *>(ctx))
    return visitAssignmentExpr(expr);
  else
    throw runtime_error("Unknown expression type");
}

shared_ptr<UnaryPrefixOp>
AstCreator::visitPrefixUnaryExpr(FluxParser::PrefixUnaryExprContext *ctx) {
  UnaryPrefixOp::Operator op;
  if (ctx->Minus())
    op = UnaryPrefixOp::Operator::Negate;
  else if (ctx->Not())
    op = UnaryPrefixOp::Operator::Not;
  else
    throw runtime_error("Unknown unary operator");

  auto expr = visitExpression(ctx->expression());
  return make_shared<UnaryPrefixOp>(Tokens(ctx), op, expr);
}

shared_ptr<ArrayReference>
AstCreator::visitArrayRefExpr(FluxParser::ArrayRefExprContext *ctx) {
  auto target = visitExpression(ctx->expression(0));
  auto index = visitExpression(ctx->expression(1));
  return make_shared<ArrayReference>(Tokens(ctx), target, index);
}

shared_ptr<VariableReference>
AstCreator::visitIdentifierExpr(FluxParser::IdentifierExprContext *ctx) {
  return make_shared<VariableReference>(Tokens(ctx),
                                        ctx->Identifier()->getText());
}

shared_ptr<BinaryLogical>
AstCreator::visitBinaryLogicalExpr(FluxParser::BinaryLogicalExprContext *ctx) {
  BinaryLogical::Operator op;
  if (ctx->LogicalAnd())
    op = BinaryLogical::Operator::And;
  else if (ctx->LogicalOr())
    op = BinaryLogical::Operator::Or;
  else
    throw runtime_error("Unknown binary logical operator");

  auto lhs = visitExpression(ctx->expression(0));
  auto rhs = visitExpression(ctx->expression(1));
  return make_shared<BinaryLogical>(Tokens(ctx), lhs, op, rhs);
}

shared_ptr<BinaryArithmetic> AstCreator::visitBinaryArithmeticExpr(
    FluxParser::BinaryArithmeticExprContext *ctx) {
  BinaryArithmetic::Operator op;
  if (ctx->Plus())
    op = BinaryArithmetic::Operator::Add;
  else if (ctx->Minus())
    op = BinaryArithmetic::Operator::Sub;
  else if (ctx->Mul())
    op = BinaryArithmetic::Operator::Mul;
  else if (ctx->Div())
    op = BinaryArithmetic::Operator::Div;
  else if (ctx->Modolu())
    op = BinaryArithmetic::Operator::Mod;
  else
    throw runtime_error("Unknown binary arithmetic operator");

  auto lhs = visitExpression(ctx->expression(0));
  auto rhs = visitExpression(ctx->expression(1));
  return make_shared<BinaryArithmetic>(Tokens(ctx), lhs, op, rhs);
}

shared_ptr<BinaryComparison>
AstCreator::visitBinaryCompExpr(FluxParser::BinaryCompExprContext *ctx) {
  BinaryComparison::Operator op;
  if (ctx->Eq())
    op = BinaryComparison::Operator::Eq;
  else if (ctx->Neq())
    op = BinaryComparison::Operator::Ne;
  else if (ctx->Less())
    op = BinaryComparison::Operator::Lt;
  else if (ctx->Leq())
    op = BinaryComparison::Operator::Le;
  else if (ctx->Greater())
    op = BinaryComparison::Operator::Gt;
  else if (ctx->Geq())
    op = BinaryComparison::Operator::Ge;
  else
    throw runtime_error("Unknown binary comparison operator");

  auto lhs = visitExpression(ctx->expression(0));
  auto rhs = visitExpression(ctx->expression(1));
  return make_shared<BinaryComparison>(Tokens(ctx), lhs, op, rhs);
}

shared_ptr<sugar::CompoundAssignment> AstCreator::visitCompoundAssignmentExpr(
    FluxParser::CompoundAssignmentExprContext *ctx) {
  BinaryArithmetic::Operator op;
  if (ctx->Plus())
    op = BinaryArithmetic::Operator::Add;
  else if (ctx->Minus())
    op = BinaryArithmetic::Operator::Sub;
  else if (ctx->Mul())
    op = BinaryArithmetic::Operator::Mul;
  else if (ctx->Div())
    op = BinaryArithmetic::Operator::Div;
  else if (ctx->Modolu())
    op = BinaryArithmetic::Operator::Mod;
  else
    throw runtime_error("Unknown compound assignment operator");

  auto lhs = visitExpression(ctx->expression(0));
  auto rhs = visitExpression(ctx->expression(1));
  return make_shared<sugar::CompoundAssignment>(Tokens(ctx), lhs, op, rhs);
}

shared_ptr<Expr>
AstCreator::visitLiteralExpr(FluxParser::LiteralExprContext *ctx) {
  return visitLiteral(ctx->literal());
}

shared_ptr<FunctionCall>
AstCreator::visitCallExpr(FluxParser::CallExprContext *ctx) {
  string name = ctx->Identifier()->getText();
  auto args = visitExpressionList(ctx->expressionList());
  return make_shared<FunctionCall>(Tokens(ctx), name, args);
}

shared_ptr<Assignment>
AstCreator::visitAssignmentExpr(FluxParser::AssignmentExprContext *ctx) {
  auto target = visitExpression(ctx->expression(0));
  auto value = visitExpression(ctx->expression(1));
  return make_shared<Assignment>(Tokens(ctx), target, value);
}

shared_ptr<Expr> AstCreator::visitParenExpr(FluxParser::ParenExprContext *ctx) {
  return visitExpression(ctx->expression());
}

shared_ptr<TernaryExpr>
AstCreator::visitTernaryExpr(FluxParser::TernaryExprContext *ctx) {
  auto condition = visitExpression(ctx->expression(0));
  auto thenExpr = visitExpression(ctx->expression(1));
  auto elseExpr = visitExpression(ctx->expression(2));
  return make_shared<TernaryExpr>(Tokens(ctx), condition, thenExpr, elseExpr);
}

shared_ptr<sugar::InIntervalExpr>
AstCreator::visitInIntervalExpr(FluxParser::InIntervalExprContext *ctx) {
  auto value = visitExpression(ctx->expression());
  auto interval = visitInterval(ctx->interval());

  return make_shared<sugar::InIntervalExpr>(Tokens(ctx), value, interval.lower,
                                            interval.upper, interval.kind);
}

shared_ptr<Expr> AstCreator::visitLiteral(FluxParser::LiteralContext *ctx) {
  if (auto intLit = ctx->IntLiteral())
    return static_pointer_cast<Expr>(
        make_shared<IntLiteral>(Tokens(ctx), stol(intLit->getText())));
  else if (auto floatLit = ctx->FloatLiteral())
    return static_pointer_cast<Expr>(
        make_shared<FloatLiteral>(Tokens(ctx), stod(floatLit->getText())));
  else if (auto stringLit = ctx->StringLiteral())
    return static_pointer_cast<Expr>(
        make_shared<StringLiteral>(Tokens(ctx), stringLit->getText()));
  else if (auto boolLit = ctx->BoolLiteral())
    return static_pointer_cast<Expr>(
        make_shared<BoolLiteral>(Tokens(ctx), boolLit->getText() == "true"));
  else
    throw runtime_error("Unknown literal type");
}

// misc
vector<shared_ptr<Expr>>
AstCreator::visitExpressionList(FluxParser::ExpressionListContext *ctx) {
  vector<shared_ptr<Expr>> expressions;
  ranges::copy(ctx->expression() | views::transform([this](auto &expr) {
                 return visitExpression(expr);
               }),
               back_inserter(expressions));
  return expressions;
}

shared_ptr<Type> AstCreator::visitType(FluxParser::TypeContext *ctx) {
  if (!ctx)
    return make_shared<InferType>();

  if (auto arrayType = ctx->arrayType())
    return visitArrayType(arrayType);
  else if (auto builtinType = ctx->builtinType())
    return visitBuiltinType(builtinType);
  else
    throw runtime_error("Unknown type");
}

shared_ptr<ArrayType>
AstCreator::visitArrayType(FluxParser::ArrayTypeContext *ctx) {
  auto elementType = visitBuiltinType(ctx->builtinType());
  long size = stol(ctx->IntLiteral()->getText());
  return make_shared<ArrayType>(elementType, size);
}

shared_ptr<Type>
AstCreator::visitBuiltinType(FluxParser::BuiltinTypeContext *ctx) {
  if (ctx->KwInt32() || ctx->KwInt64())
    return make_shared<IntType>();
  else if (ctx->KwFloat32() || ctx->KwFloat64())
    return make_shared<FloatType>();
  else if (ctx->KwBool())
    return make_shared<BoolType>();
  else if (ctx->KwString())
    return make_shared<StringType>();
  else
    throw runtime_error("Unknown builtin type");
}

Interval AstCreator::visitInterval(FluxParser::IntervalContext *ctx) {
  auto lower = visitExpression(ctx->expression(0));
  auto upper = visitExpression(ctx->expression(1));

  sugar::InIntervalExpr::IntervalKind kind;
  if (ctx->LeftParen() && ctx->RightParen())
    kind = sugar::InIntervalExpr::IntervalKind::Open;
  else if (ctx->LeftParen() && ctx->RightBracket())
    kind = sugar::InIntervalExpr::IntervalKind::OpenClosed;
  else if (ctx->LeftBracket() && ctx->RightParen())
    kind = sugar::InIntervalExpr::IntervalKind::ClosedOpen;
  else if (ctx->LeftBracket() && ctx->RightBracket())
    kind = sugar::InIntervalExpr::IntervalKind::Closed;
  else
    throw runtime_error("Unknown interval kind");
  return Interval{lower, upper, kind};
}

// classes
any AstCreator::visitClassDefinition(FluxParser::ClassDefinitionContext *ctx) {
  throw runtime_error("Not implemented");
}

any AstCreator::visitFieldDeclaration(
    FluxParser::FieldDeclarationContext *ctx) {
  throw runtime_error("Not implemented");
}