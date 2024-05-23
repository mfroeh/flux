#include "ast/ast_creator.hh"
#include "ast/class.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"
#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <memory>
#include <ranges>

using namespace std;
using namespace ranges;

AstCreator::AstCreator(ModuleContext &moduleContext)
    : moduleContext(moduleContext) {}

// module
Module AstCreator::visitModule(FP::ModuleContext *ctx) {
  vector<filesystem::path> includes;
  ranges::copy(ctx->includes() | views::transform([this](auto &include) {
                 string text = include->Path()->getText();
                 if (text.starts_with("./")) {
                   return moduleContext.path.parent_path() / text.substr(2);
                 } else {
                   return filesystem::path(text);
                 }
               }),
               back_inserter(includes));

  vector<ClassDefinition> classes;
  ranges::copy(ctx->classDefinition() |
                   views::transform([this](auto &classDef) {
                     return visitClassDefinition(classDef);
                   }),
               back_inserter(classes));

  vector<FunctionDefinition> functions;
  ranges::copy(ctx->functionDefinition() | views::transform([this](auto &func) {
                 return visitFunctionDefinition(func);
               }),
               back_inserter(functions));

  return Module(Tokens(ctx), moduleContext.path, includes, classes, functions);
}

// classes
ClassDefinition
AstCreator::visitClassDefinition(FP::ClassDefinitionContext *ctx) {
  string name = ctx->Identifier()->getText();
  vector<FieldDeclaration> fields;
  ranges::copy(ctx->fieldDeclaration() | views::transform([this](auto &field) {
                 return visitFieldDeclaration(field);
               }),
               back_inserter(fields));

  vector<FunctionDefinition> methods;
  ranges::copy(ctx->functionDefinition() | views::transform([this](auto &func) {
                 return visitFunctionDefinition(func);
               }),
               back_inserter(methods));

  return ClassDefinition(Tokens(ctx), name, fields, methods);
}

FieldDeclaration
AstCreator::visitFieldDeclaration(FP::FieldDeclarationContext *ctx) {
  assert(ctx->type());
  string name = ctx->Identifier()->getText();
  auto type = visitType(ctx->type());

  // todo: default initializer for fields
  // auto initializer =
  //     ctx->expression() ? visitExpression(ctx->expression()) : nullptr;
  return FieldDeclaration(Tokens(ctx), name, type);
}

// functions
FunctionDefinition
AstCreator::visitFunctionDefinition(FP::FunctionDefinitionContext *ctx) {
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
AstCreator::visitParameterList(FP::ParameterListContext *ctx) {
  vector<Parameter> parameters;
  if (!ctx)
    return parameters;
  ranges::copy(ctx->parameter() | views::transform([this](auto &param) {
                 return visitParameter(param);
               }),
               back_inserter(parameters));
  return parameters;
}
Parameter AstCreator::visitParameter(FP::ParameterContext *ctx) {
  string name = ctx->Identifier()->getText();
  auto type = visitType(ctx->type());
  return Parameter(Tokens(ctx), name, type);
}

// statements
Block AstCreator::visitBlock(FP::BlockContext *ctx) {
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

Block AstCreator::visitSingleLineBody(FP::StatementContext *ctx) {
  auto statement = visitStatement(ctx);
  return Block(statement->tokens, vector<shared_ptr<Statement>>{statement},
               false);
}

shared_ptr<Statement> AstCreator::visitStatement(FP::StatementContext *ctx) {
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
  else if (auto standaloneBlock = ctx->standaloneBlock())
    return visitStandaloneBlock(standaloneBlock);
  else if (auto functioDef = ctx->functionDefinition())
    return make_shared<FunctionDefinition>(visitFunctionDefinition(functioDef));
  else
    throw runtime_error("Unknown statement type");
}

shared_ptr<ExpressionStatement>
AstCreator::visitExpressionStatement(FP::ExpressionStatementContext *ctx) {
  return make_shared<ExpressionStatement>(Tokens(ctx),
                                          visitExpression(ctx->expression()));
}

shared_ptr<VariableDeclaration>
AstCreator::visitVariableDeclaration(FP::VariableDeclarationContext *ctx) {
  string name = ctx->Identifier()->getText();
  auto type = visitType(ctx->type());
  shared_ptr<Expr> initializer;
  if (ctx->expression())
    initializer = visitExpression(ctx->expression());
  return make_shared<VariableDeclaration>(Tokens(ctx), name, type, initializer);
}

shared_ptr<Return>
AstCreator::visitReturnStatement(FP::ReturnStatementContext *ctx) {
  return make_shared<Return>(Tokens(ctx), visitExpression(ctx->expression()));
}

shared_ptr<While> AstCreator::visitWhileLoop(FP::WhileLoopContext *ctx) {
  auto condition = visitExpression(ctx->expression());
  auto body = ctx->block() ? visitBlock(ctx->block())
                           : visitSingleLineBody(ctx->statement());

  return make_shared<While>(Tokens(ctx), condition, body);
}

shared_ptr<StandaloneBlock>
AstCreator::visitStandaloneBlock(FP::StandaloneBlockContext *ctx) {
  return make_shared<StandaloneBlock>(Tokens(ctx), visitBlock(ctx->block()));
}

shared_ptr<sugar::ForLoop> AstCreator::visitForLoop(FP::ForLoopContext *ctx) {
  auto init = visitStatement(ctx->statement(0)); // let i = 0;
  auto conditionStatement =
      visitExpressionStatement(ctx->expressionStatement()); // i < N;
  auto condition = conditionStatement->expr;
  auto post = visitStatement(ctx->statement(1)); // i += 1;

  auto body = ctx->block() ? visitBlock(ctx->block())
                           : visitSingleLineBody(ctx->statement(2));

  return make_shared<sugar::ForLoop>(Tokens(ctx), init, condition, post, body);
}

shared_ptr<sugar::IfElifElseStatement>
AstCreator::visitIfStatement(FP::IfStatementContext *ctx) {
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
AstCreator::visitElseIfStatement(FP::ElseIfStatementContext *ctx) {
  auto condition = visitExpression(ctx->expression());
  auto thenBlock = ctx->block() ? visitBlock(ctx->block())
                                : visitSingleLineBody(ctx->statement());
  return make_shared<sugar::ElifStatement>(Tokens(ctx), condition, thenBlock);
}

Block AstCreator::visitElseBlock(FP::ElseBlockContext *ctx) {
  return visitBlock(ctx->block());
}

// expressions
shared_ptr<Expr> AstCreator::visitExpression(FP::ExpressionContext *ctx) {
  // E.g. ret;
  if (!ctx)
    return make_shared<VoidExpr>(Tokens());

  using FP = FP;
  if (auto expr = dynamic_cast<FP::ParenExprContext *>(ctx))
    return visitParenExpr(expr);
  else if (auto expr = dynamic_cast<FP::LiteralExprContext *>(ctx))
    return visitLiteralExpr(expr);
  else if (auto expr = dynamic_cast<FP::VarRefContext *>(ctx))
    return visitVarRef(expr);
  else if (auto expr = dynamic_cast<FP::FieldRefContext *>(ctx))
    return visitFieldRef(expr);
  else if (auto expr = dynamic_cast<FP::ArrayRefContext *>(ctx))
    return visitArrayRef(expr);
  else if (auto expr = dynamic_cast<FP::FunctionCallContext *>(ctx))
    return visitFunctionCall(expr);
  else if (auto expr = dynamic_cast<FP::MethodCallContext *>(ctx))
    return visitMethodCall(expr);
  else if (auto expr = dynamic_cast<FP::PrefixUnaryContext *>(ctx))
    return visitPrefixUnary(expr);
  else if (auto expr = dynamic_cast<FP::BinaryLogicalContext *>(ctx))
    return visitBinaryLogical(expr);
  else if (auto expr = dynamic_cast<FP::BinaryArithmeticContext *>(ctx))
    return visitBinaryArithmetic(expr);
  else if (auto expr = dynamic_cast<FP::BinaryCompContext *>(ctx))
    return visitBinaryComp(expr);
  else if (auto expr = dynamic_cast<FP::TernaryContext *>(ctx))
    return visitTernary(expr);
  else if (auto expr = dynamic_cast<FP::InIntervalContext *>(ctx))
    return visitInInterval(expr);
  else if (auto expr = dynamic_cast<FP::CompoundAssignmentContext *>(ctx))
    return visitCompoundAssignment(expr);
  else if (auto expr = dynamic_cast<FP::AssignmentContext *>(ctx))
    return visitAssignment(expr);
  else if (auto arrayInit = dynamic_cast<FP::ArrayLiteralContext *>(ctx))
    return visitArrayLiteral(arrayInit);
  else if (auto structLit = dynamic_cast<FP::StructLiteralContext *>(ctx))
    return visitStructLiteral(structLit);
  else if (auto malloc = dynamic_cast<FP::MallocContext *>(ctx))
    return visitMalloc(malloc);
  else
    throw runtime_error("Unknown expression type");
}

shared_ptr<Expr> AstCreator::visitPrefixUnary(FP::PrefixUnaryContext *ctx) {
  UnaryPrefixOp::Operator op;
  if (ctx->Minus())
    op = UnaryPrefixOp::Operator::Negate;
  else if (ctx->Not())
    op = UnaryPrefixOp::Operator::Not;
  else if (ctx->Mul())
    return make_shared<Dereference>(Tokens(ctx),
                                    visitExpression(ctx->expression()));
  else if (ctx->Reference())
    return make_shared<Pointer>(Tokens(ctx),
                                visitExpression(ctx->expression()));
  else
    throw runtime_error("Unknown unary operator");

  auto expr = visitExpression(ctx->expression());
  return make_shared<UnaryPrefixOp>(Tokens(ctx), op, expr);
}

shared_ptr<ArrayRef> AstCreator::visitArrayRef(FP::ArrayRefContext *ctx) {
  // the indexable expr
  auto target = visitExpression(ctx->expression());

  auto indices = visitExpressionList(ctx->expressionList());
  for (unsigned i = 0; i < indices.size(); i++) {
    target = make_shared<ArrayRef>(Tokens(ctx), target, indices[i]);
  }

  return static_pointer_cast<ArrayRef>(target);
}

shared_ptr<VarRef> AstCreator::visitVarRef(FP::VarRefContext *ctx) {
  return make_shared<VarRef>(Tokens(ctx), ctx->Identifier()->getText());
}

shared_ptr<FieldRef> AstCreator::visitFieldRef(FP::FieldRefContext *ctx) {
  auto obj = visitExpression(ctx->expression());
  // if expr -> name
  if (ctx->RightArrow())
    obj = make_shared<Dereference>(Tokens(ctx), obj);

  return make_shared<FieldRef>(Tokens(ctx), obj, ctx->Identifier()->getText());
}

shared_ptr<BinaryLogical>
AstCreator::visitBinaryLogical(FP::BinaryLogicalContext *ctx) {
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

shared_ptr<BinaryArithmetic>
AstCreator::visitBinaryArithmetic(FP::BinaryArithmeticContext *ctx) {
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
AstCreator::visitBinaryComp(FP::BinaryCompContext *ctx) {
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

shared_ptr<sugar::CompoundAssignment>
AstCreator::visitCompoundAssignment(FP::CompoundAssignmentContext *ctx) {
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
  lhs->setLhs(true);
  auto rhs = visitExpression(ctx->expression(1));
  return make_shared<sugar::CompoundAssignment>(Tokens(ctx), lhs, op, rhs);
}

shared_ptr<Expr> AstCreator::visitLiteralExpr(FP::LiteralExprContext *ctx) {
  return visitLiteral(ctx->literal());
}

shared_ptr<FunctionCall>
AstCreator::visitFunctionCall(FP::FunctionCallContext *ctx) {
  auto args = visitExpressionList(ctx->expressionList());
  return make_shared<FunctionCall>(Tokens(ctx), ctx->Identifier()->getText(),
                                   args);
}

shared_ptr<MethodCall> AstCreator::visitMethodCall(FP::MethodCallContext *ctx) {
  auto object = visitExpression(ctx->expression());
  // if expr -> name
  if (ctx->RightArrow())
    object = make_shared<Dereference>(Tokens(ctx), object);

  auto args = visitExpressionList(ctx->expressionList());
  return make_shared<MethodCall>(Tokens(ctx), object,
                                 ctx->Identifier()->getText(), args);
}

shared_ptr<Assignment> AstCreator::visitAssignment(FP::AssignmentContext *ctx) {
  auto target = visitExpression(ctx->expression(0));
  target->setLhs(true);
  auto value = visitExpression(ctx->expression(1));
  return make_shared<Assignment>(Tokens(ctx), target, value);
}

shared_ptr<Expr> AstCreator::visitParenExpr(FP::ParenExprContext *ctx) {
  return visitExpression(ctx->expression());
}

shared_ptr<TernaryExpr> AstCreator::visitTernary(FP::TernaryContext *ctx) {
  auto condition = visitExpression(ctx->expression(0));
  auto thenExpr = visitExpression(ctx->expression(1));
  auto elseExpr = visitExpression(ctx->expression(2));
  return make_shared<TernaryExpr>(Tokens(ctx), condition, thenExpr, elseExpr);
}

shared_ptr<sugar::InIntervalExpr>
AstCreator::visitInInterval(FP::InIntervalContext *ctx) {
  auto value = visitExpression(ctx->expression());
  auto interval = visitInterval(ctx->interval());

  return make_shared<sugar::InIntervalExpr>(Tokens(ctx), value, interval.lower,
                                            interval.upper, interval.kind);
}

shared_ptr<Expr> AstCreator::visitLiteral(FP::LiteralContext *ctx) {
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

shared_ptr<Expr> AstCreator::visitArrayLiteral(FP::ArrayLiteralContext *ctx) {
  return make_shared<ArrayLiteral>(Tokens(ctx),
                                   visitExpressionList(ctx->expressionList()));
}

shared_ptr<Expr> AstCreator::visitStructLiteral(FP::StructLiteralContext *ctx) {
  string type = ctx->Identifier(0)->getText();

  vector<string> names;
  ranges::copy(ctx->Identifier() |
                   views::transform([](auto &id) { return id->getText(); }),
               back_inserter(names));
  // remove type
  names.erase(names.begin());

  vector<shared_ptr<Expr>> values;
  ranges::copy(ctx->expression() | views::transform([this](auto &expr) {
                 return visitExpression(expr);
               }),
               back_inserter(values));

  return make_shared<StructLiteral>(Tokens(ctx), type, names, values);
}

shared_ptr<Expr> AstCreator::visitMalloc(FP::MallocContext *ctx) {
  auto type = visitType(ctx->type());
  auto count = visitExpression(ctx->expression());
  return make_shared<Halloc>(Tokens(ctx), type, count);
}

// misc
vector<shared_ptr<Expr>>
AstCreator::visitExpressionList(FP::ExpressionListContext *ctx) {
  vector<shared_ptr<Expr>> expressions;
  if (!ctx)
    return expressions;
  ranges::copy(ctx->expression() | views::transform([this](auto &expr) {
                 return visitExpression(expr);
               }),
               back_inserter(expressions));
  return expressions;
}

// types
shared_ptr<Type> AstCreator::visitType(FP::TypeContext *ctx) {
  if (!ctx)
    return InferType::get();

  if (auto scalar = ctx->scalarType())
    return visitScalarType(scalar);
  else if (auto arrayType = ctx->arrayType())
    return visitArrayType(arrayType);
  else if (auto pointerType = ctx->pointerType())
    return visitPointerType(pointerType);
  else
    assert(false && "unknown type");
}

shared_ptr<PointerType>
AstCreator::visitPointerType(FP::PointerTypeContext *ctx) {
  shared_ptr<Type> pointee;
  if (auto scalar = ctx->scalarType())
    pointee = visitScalarType(scalar);
  else if (auto array = ctx->arrayType())
    pointee = visitArrayType(array);
  else
    assert(false && "Unknown pointer type");

  shared_ptr<Type> type = pointee;
  for (unsigned i = 0; i < ctx->Mul().size(); i++) {
    type = PointerType::get(type);
  }
  return static_pointer_cast<PointerType>(type);
}

shared_ptr<ArrayType> AstCreator::visitArrayType(FP::ArrayTypeContext *ctx) {
  auto type = visitScalarType(ctx->scalarType());
  for (unsigned i = 0; i < ctx->Mul().size(); i++) {
    type = PointerType::get(type);
  }

  for (auto &sizeDecl : ctx->IntLiteral()) {
    long size = stol(sizeDecl->getText());
    type = ArrayType::get(type, size);
  }
  return static_pointer_cast<ArrayType>(type);
}

shared_ptr<Type> AstCreator::visitBuiltinType(FP::BuiltinTypeContext *ctx) {
  if (ctx->KwInt32() || ctx->KwInt64())
    return IntType::get();
  else if (ctx->KwFloat32() || ctx->KwFloat64())
    return FloatType::get();
  else if (ctx->KwBool())
    return BoolType::get();
  else if (ctx->KwString())
    return StringType::get();
  else if (ctx->KwVoid())
    return VoidType::get();
  else
    assert(false && "Unknown builtin type");
}

shared_ptr<Type> AstCreator::visitScalarType(FP::ScalarTypeContext *ctx) {
  if (ctx->builtinType())
    return visitBuiltinType(ctx->builtinType());
  else if (ctx->Identifier())
    return ClassType::get(ctx->Identifier()->getText());
  else
    assert(false && "Unknown scalar type");
}

Interval AstCreator::visitInterval(FP::IntervalContext *ctx) {
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
