#include "analysis/desugar.hh"
#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"

using namespace std;

// module
any Desugarer::visit(Module &module) {
  for (auto &function : module.functions) {
    any res = function.accept(*this);
    if (res.has_value())
      function = *any_cast<FunctionDefinition *>(res);
  }
  return {};
}

// functions
any Desugarer::visit(FunctionDefinition &function) {
  for (auto &parameter : function.parameters) {
    any res = parameter.accept(*this);
    if (res.has_value())
      parameter = any_cast<Parameter &>(res);
  }
  any res = function.body.accept(*this);
  if (res.has_value())
    function.body = *any_cast<Block *>(res);
  return {};
}

any Desugarer::visit(Parameter &parameter) { return {}; }

// statements
any Desugarer::visit(Block &block) {
  for (auto &stmt : block.statements) {
    any res = stmt->accept(*this);
    if (res.has_value()) {
      cout << "Desugared " << *stmt << endl;
      stmt.reset(any_cast<Statement *>(res));
    }
  }
  return {};
}

any Desugarer::visit(Return &ret) {
  any res = ret.expression->accept(*this);
  if (res.has_value())
    ret.expression.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(IfElse &ifElse) {
  any res = ifElse.condition->accept(*this);
  if (res.has_value())
    ifElse.condition.reset(any_cast<Expr *>(res));
  res = ifElse.thenBlock.accept(*this);
  if (res.has_value())
    ifElse.thenBlock = *any_cast<Block *>(res);

  res = ifElse.elseBlock.accept(*this);
  if (res.has_value())
    ifElse.elseBlock = *any_cast<Block *>(res);
  return {};
}

any Desugarer::visit(While &whileStmt) {
  any res = whileStmt.condition->accept(*this);
  if (res.has_value())
    whileStmt.condition.reset(any_cast<Expr *>(res));
  res = whileStmt.body.accept(*this);
  if (res.has_value())
    whileStmt.body = *any_cast<Block *>(res);
  return {};
}

any Desugarer::visit(ExpressionStatement &exprStmt) {
  any res = exprStmt.expression->accept(*this);
  if (res.has_value())
    exprStmt.expression.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(VariableDeclaration &varDecl) {
  if (varDecl.initializer) {
    any res = varDecl.initializer->accept(*this);
    if (res.has_value())
      varDecl.initializer.reset(any_cast<Expr *>(res));
  }
  return {};
}

// expressions

any Desugarer::visit(Cast &cast) {
  any res = cast.expr->accept(*this);
  if (res.has_value())
    cast.expr.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(IntLiteral &intLit) { return {}; }

any Desugarer::visit(FloatLiteral &floatLit) { return {}; }

any Desugarer::visit(BoolLiteral &boolLit) { return {}; }

any Desugarer::visit(StringLiteral &stringLit) { return {}; }

any Desugarer::visit(VariableReference &var) { return {}; }

any Desugarer::visit(ArrayReference &arr) {
  any res = arr.index->accept(*this);
  if (res.has_value())
    arr.index.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(FunctionCall &funcCall) {
  for (auto &arg : funcCall.arguments) {
    any res = arg->accept(*this);
    if (res.has_value())
      arg.reset(any_cast<Expr *>(res));
  }
  return {};
}

any Desugarer::visit(UnaryPrefixOp &unaryOp) {
  any res = unaryOp.operand->accept(*this);
  if (res.has_value())
    unaryOp.operand.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(BinaryArithmetic &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs.reset(any_cast<Expr *>(res));
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(BinaryComparison &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs.reset(any_cast<Expr *>(res));
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(BinaryLogical &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs.reset(any_cast<Expr *>(res));
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(TernaryExpr &ternaryOp) {
  any res = ternaryOp.condition->accept(*this);
  if (res.has_value())
    ternaryOp.condition.reset(any_cast<Expr *>(res));
  res = ternaryOp.thenExpr->accept(*this);
  if (res.has_value())
    ternaryOp.thenExpr.reset(any_cast<Expr *>(res));
  res = ternaryOp.elseExpr->accept(*this);
  if (res.has_value())
    ternaryOp.elseExpr.reset(any_cast<Expr *>(res));
  return {};
}

any Desugarer::visit(Assignment &assignment) {
  any res = assignment.target->accept(*this);
  if (res.has_value())
    assignment.target.reset(any_cast<Expr *>(res));
  res = assignment.value->accept(*this);
  if (res.has_value())
    assignment.value.reset(any_cast<Expr *>(res));
  return {};
}

// sugar
any Desugarer::visit(sugar::ElifStatement &elifStmt) {
  any res = elifStmt.condition->accept(*this);
  if (res.has_value())
    elifStmt.condition.reset(any_cast<Expr *>(res));

  res = elifStmt.thenBlock.accept(*this);
  if (res.has_value())
    elifStmt.thenBlock = *any_cast<Block *>(res);

  return new IfElse(elifStmt.tokens, elifStmt.condition, elifStmt.thenBlock,
                    Block());
}

any Desugarer::visit(sugar::IfElifElseStatement &elifElseStmt) {
  // first resolve all the other sugar
  any res = elifElseStmt.thenBlock.accept(*this);
  if (res.has_value())
    elifElseStmt.thenBlock = *any_cast<Block *>(res);

  res = elifElseStmt.elseBlock.accept(*this);
  if (res.has_value())
    elifElseStmt.elseBlock = *any_cast<Block *>(res);

  // then desugar itself
  auto toplevel = new IfElse(elifElseStmt.tokens, elifElseStmt.condition,
                             elifElseStmt.thenBlock, Block());
  auto cur = toplevel;
  for (auto &elif : elifElseStmt.elseIfs) {
    auto ifElse = any_cast<IfElse *>(elif->accept(*this));
    auto stmts = vector<shared_ptr<Statement>>{shared_ptr<IfElse>(ifElse)};
    cur->elseBlock = Block(ifElse->tokens, stmts, false);
  }

  cur->elseBlock = elifElseStmt.elseBlock;

  return toplevel;
}

any Desugarer::visit(sugar::ForLoop &forStmt) {
  // first resolve all the other sugar
  any res = forStmt.initializer->accept(*this);
  if (res.has_value())
    forStmt.initializer.reset(any_cast<Statement *>(res));
  res = forStmt.condition->accept(*this);
  if (res.has_value())
    forStmt.condition.reset(any_cast<Expr *>(res));
  res = forStmt.update->accept(*this);
  if (res.has_value())
    forStmt.update.reset(any_cast<Statement *>(res));
  res = forStmt.body.accept(*this);
  if (res.has_value())
    forStmt.body = *any_cast<Block *>(res);

  // then desugar itself
  // TODO
  return {};
}

any Desugarer::visit(sugar::InIntervalExpr &inIntervalExpr) {
  // first resolve all the other sugar
  any res = inIntervalExpr.value->accept(*this);
  if (res.has_value())
    inIntervalExpr.value.reset(any_cast<Expr *>(res));
  res = inIntervalExpr.lower->accept(*this);
  if (res.has_value())
    inIntervalExpr.lower.reset(any_cast<Expr *>(res));
  res = inIntervalExpr.upper->accept(*this);
  if (res.has_value())
    inIntervalExpr.upper.reset(any_cast<Expr *>(res));

  // then desugar itself
  // i in [0, 10] -> i >= 0 && i <= 10
  auto lower = inIntervalExpr.lower;
  auto upper = inIntervalExpr.upper;

  auto lowerOperator =
      inIntervalExpr.kind == sugar::InIntervalExpr::Open ||
              inIntervalExpr.kind == sugar::InIntervalExpr::OpenClosed
          ? BinaryComparison::Operator::Gt
          : BinaryComparison::Operator::Ge;

  auto upperOperator =
      inIntervalExpr.kind == sugar::InIntervalExpr::Open ||
              inIntervalExpr.kind == sugar::InIntervalExpr::ClosedOpen
          ? BinaryComparison::Operator::Lt
          : BinaryComparison::Operator::Le;

  auto lowerComparison = make_shared<BinaryComparison>(
      inIntervalExpr.tokens, inIntervalExpr.value, lowerOperator, lower);
  auto upperComparison = make_shared<BinaryComparison>(
      inIntervalExpr.tokens, inIntervalExpr.value, upperOperator, upper);

  return new BinaryLogical(inIntervalExpr.tokens, lowerComparison,
                           BinaryLogical::Operator::And, upperComparison);
}

any Desugarer::visit(sugar::CompoundAssignment &compoundAssignment) {
  // first resolve all the other sugar
  any res = compoundAssignment.target->accept(*this);
  if (res.has_value())
    compoundAssignment.target.reset(any_cast<Expr *>(res));
  res = compoundAssignment.value->accept(*this);
  if (res.has_value())
    compoundAssignment.value.reset(any_cast<Expr *>(res));

  // then desugar itself
  // i += 1 -> i = i + 1
  auto reference = shared_ptr<Expr>(compoundAssignment.target.get());
  auto operation = make_shared<BinaryArithmetic>(
      compoundAssignment.tokens, reference, compoundAssignment.op,
      compoundAssignment.value);

  return new Assignment(compoundAssignment.tokens, reference, operation);
}