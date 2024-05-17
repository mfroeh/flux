#include "analysis/desugar.hh"
#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include <any>
#include <memory>

using namespace std;

// module
any Desugarer::visit(Module &module) {
  for (auto &function : module.functions) {
    any res = function.accept(*this);
    if (res.has_value())
      function = any_cast<FunctionDefinition>(res);
  }
  return {};
}

// functions
any Desugarer::visit(FunctionDefinition &function) {
  for (auto &parameter : function.parameters) {
    any res = parameter.accept(*this);
    if (res.has_value())
      parameter = any_cast<Parameter>(res);
  }
  any res = function.body.accept(*this);
  if (res.has_value())
    function.body = any_cast<Block>(res);
  return {};
}

any Desugarer::visit(Parameter &parameter) { return {}; }

// statements
any Desugarer::visit(Block &block) {
  for (auto &stmt : block.statements) {
    any res = stmt->accept(*this);
    if (res.has_value()) {
      stmt = any_cast<shared_ptr<Statement>>(res);
    }
  }
  return {};
}

any Desugarer::visit(Return &ret) {
  any res = ret.expression->accept(*this);
  if (res.has_value())
    ret.expression = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(IfElse &ifElse) {
  any res = ifElse.condition->accept(*this);
  if (res.has_value())
    ifElse.condition = any_cast<shared_ptr<Expr>>(res);
  res = ifElse.thenBlock.accept(*this);
  if (res.has_value())
    ifElse.thenBlock = any_cast<Block>(res);

  res = ifElse.elseBlock.accept(*this);
  if (res.has_value())
    ifElse.elseBlock = any_cast<Block>(res);
  return {};
}

any Desugarer::visit(While &whileStmt) {
  any res = whileStmt.condition->accept(*this);
  if (res.has_value())
    whileStmt.condition = any_cast<shared_ptr<Expr>>(res);
  res = whileStmt.body.accept(*this);
  if (res.has_value())
    whileStmt.body = any_cast<Block>(res);
  return {};
}

any Desugarer::visit(ExpressionStatement &exprStmt) {
  any res = exprStmt.expr->accept(*this);
  if (res.has_value())
    exprStmt.expr = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(VariableDeclaration &varDecl) {
  if (varDecl.initializer) {
    any res = varDecl.initializer->accept(*this);
    if (res.has_value())
      varDecl.initializer = any_cast<shared_ptr<Expr>>(res);
  }
  return {};
}

any Desugarer::visit(StandaloneBlock &standaloneBlock) {
  any res = standaloneBlock.block.accept(*this);
  if (res.has_value())
    standaloneBlock.block = any_cast<Block>(res);
  return {};
}

// expressions

any Desugarer::visit(Cast &cast) {
  any res = cast.expr->accept(*this);
  if (res.has_value())
    cast.expr = any_cast<shared_ptr<Expr>>(res);
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
    arr.index = any_cast<shared_ptr<Expr>>(res);
  res = arr.arrayExpr->accept(*this);
  if (res.has_value())
    arr.arrayExpr = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(FunctionCall &funcCall) {
  for (auto &arg : funcCall.arguments) {
    any res = arg->accept(*this);
    if (res.has_value())
      arg = any_cast<shared_ptr<Expr>>(res);
  }
  return {};
}

any Desugarer::visit(UnaryPrefixOp &unaryOp) {
  any res = unaryOp.operand->accept(*this);
  if (res.has_value())
    unaryOp.operand = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(BinaryArithmetic &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs = any_cast<shared_ptr<Expr>>(res);
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(BinaryComparison &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs = any_cast<shared_ptr<Expr>>(res);
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(BinaryLogical &binaryOp) {
  any res = binaryOp.lhs->accept(*this);
  if (res.has_value())
    binaryOp.lhs = any_cast<shared_ptr<Expr>>(res);
  res = binaryOp.rhs->accept(*this);
  if (res.has_value())
    binaryOp.rhs = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(TernaryExpr &ternaryOp) {
  any res = ternaryOp.condition->accept(*this);
  if (res.has_value())
    ternaryOp.condition = any_cast<shared_ptr<Expr>>(res);
  res = ternaryOp.thenExpr->accept(*this);
  if (res.has_value())
    ternaryOp.thenExpr = any_cast<shared_ptr<Expr>>(res);
  res = ternaryOp.elseExpr->accept(*this);
  if (res.has_value())
    ternaryOp.elseExpr = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(Assignment &assignment) {
  any res = assignment.target->accept(*this);
  if (res.has_value())
    assignment.target = any_cast<shared_ptr<Expr>>(res);
  res = assignment.value->accept(*this);
  if (res.has_value())
    assignment.value = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(Pointer &pointer) {
  any res = pointer.lvalue->accept(*this);
  if (res.has_value())
    pointer.lvalue = any_cast<shared_ptr<Expr>>(res);
  return {};
}

any Desugarer::visit(Dereference &dereference) {
  any res = dereference.pointer->accept(*this);
  if (res.has_value())
    dereference.pointer = any_cast<shared_ptr<Expr>>(res);
  return {};
}

// sugar
any Desugarer::visit(sugar::ElifStatement &elifStmt) {
  any res = elifStmt.condition->accept(*this);
  if (res.has_value())
    elifStmt.condition = any_cast<shared_ptr<Expr>>(res);

  res = elifStmt.thenBlock.accept(*this);
  if (res.has_value())
    elifStmt.thenBlock = any_cast<Block>(res);

  return make_shared<IfElse>(elifStmt.tokens, elifStmt.condition,
                             elifStmt.thenBlock, Block());
}

any Desugarer::visit(sugar::IfElifElseStatement &elifElseStmt) {
  // first resolve all the other sugar
  any res = elifElseStmt.thenBlock.accept(*this);
  if (res.has_value())
    elifElseStmt.thenBlock = any_cast<Block>(res);

  res = elifElseStmt.elseBlock.accept(*this);
  if (res.has_value())
    elifElseStmt.elseBlock = any_cast<Block>(res);

  // then desugar itself
  auto toplevel =
      make_shared<IfElse>(elifElseStmt.tokens, elifElseStmt.condition,
                          elifElseStmt.thenBlock, Block());
  auto cur = toplevel;
  for (auto &elif : elifElseStmt.elseIfs) {
    auto ifElse = any_cast<shared_ptr<IfElse>>(elif->accept(*this));
    auto stmts = vector<shared_ptr<Statement>>{shared_ptr<IfElse>(ifElse)};
    cur->elseBlock = Block(ifElse->tokens, stmts, false);
  }

  cur->elseBlock = elifElseStmt.elseBlock;
  return static_pointer_cast<Statement>(toplevel);
}

any Desugarer::visit(sugar::ForLoop &forStmt) {
  // first resolve all the other sugar
  any res = forStmt.initializer->accept(*this);
  if (res.has_value())
    forStmt.initializer = any_cast<shared_ptr<Statement>>(res);
  res = forStmt.condition->accept(*this);
  if (res.has_value())
    forStmt.condition = any_cast<shared_ptr<Expr>>(res);
  res = forStmt.update->accept(*this);
  if (res.has_value())
    forStmt.update = any_cast<shared_ptr<Statement>>(res);
  res = forStmt.body.accept(*this);
  if (res.has_value())
    forStmt.body = any_cast<Block>(res);

  // then desugar itself
  forStmt.body.statements.push_back(forStmt.update);
  auto whileLoop =
      make_shared<While>(forStmt.tokens, forStmt.condition, forStmt.body);

  auto forBlock = Block(forStmt.tokens, {forStmt.initializer, whileLoop}, true);
  auto standalone = make_shared<StandaloneBlock>(forStmt.tokens, forBlock);
  return static_pointer_cast<Statement>(standalone);
}

any Desugarer::visit(sugar::InIntervalExpr &inIntervalExpr) {
  // first resolve all the other sugar
  any res = inIntervalExpr.value->accept(*this);
  if (res.has_value())
    inIntervalExpr.value = any_cast<shared_ptr<Expr>>(res);
  res = inIntervalExpr.lower->accept(*this);
  if (res.has_value())
    inIntervalExpr.lower = any_cast<shared_ptr<Expr>>(res);
  res = inIntervalExpr.upper->accept(*this);
  if (res.has_value())
    inIntervalExpr.upper = any_cast<shared_ptr<Expr>>(res);

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

  shared_ptr<Expr> valueCopy;
  if (auto var =
          dynamic_pointer_cast<VariableReference>(inIntervalExpr.value)) {
    valueCopy = make_shared<VariableReference>(*var);
  } else if (auto arr =
                 dynamic_pointer_cast<ArrayReference>(inIntervalExpr.value)) {
    valueCopy = make_shared<ArrayReference>(*arr);
  } else {
    throw runtime_error("Only variables and array references are allowed for "
                        "in interval expressions");
  }

  auto lowerComparison = make_shared<BinaryComparison>(
      inIntervalExpr.tokens, inIntervalExpr.value, lowerOperator, lower);
  auto upperComparison = make_shared<BinaryComparison>(
      inIntervalExpr.tokens, valueCopy, upperOperator, upper);

  auto logical =
      make_shared<BinaryLogical>(inIntervalExpr.tokens, lowerComparison,
                                 BinaryLogical::Operator::And, upperComparison);
  return static_pointer_cast<Expr>(logical);
}

any Desugarer::visit(sugar::CompoundAssignment &compoundAssignment) {
  // first resolve all the other sugar
  any res = compoundAssignment.target->accept(*this);
  if (res.has_value())
    compoundAssignment.target = any_cast<shared_ptr<Expr>>(res);
  res = compoundAssignment.value->accept(*this);
  if (res.has_value())
    compoundAssignment.value = any_cast<shared_ptr<Expr>>(res);

  // then desugar itself
  // i += 1 -> i = i + 1
  auto lvalue = compoundAssignment.target;
  auto lvalueCopy = lvalue->deepcopy();
  lvalueCopy->setLhs(false);

  auto operation = make_shared<BinaryArithmetic>(
      compoundAssignment.tokens, lvalueCopy, compoundAssignment.op,
      compoundAssignment.value);

  auto assignment =
      make_shared<Assignment>(compoundAssignment.tokens, lvalue, operation);
  return static_pointer_cast<Expr>(assignment);
}