#include "visitor.hh"

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"

using namespace std;

// module
any AstVisitor::visit(Module &module) {
  for (auto &function : module.functions) {
    function.accept(*this);
  }
  return {};
}

// functions
any AstVisitor::visit(FunctionDefinition &function) {
  for (auto &parameter : function.parameters) {
    parameter.accept(*this);
  }
  function.body.accept(*this);
  return {};
}

any AstVisitor::visit(Parameter &parameter) { return {}; }

// statements
any AstVisitor::visit(Block &block) {
  for (auto &stmt : block.statements) {
    cout << *stmt << endl;
    stmt->accept(*this);
  }
  return {};
}

any AstVisitor::visit(Return &ret) {
  ret.expression->accept(*this);
  return {};
}

any AstVisitor::visit(IfElse &ifElse) {
  ifElse.condition->accept(*this);
  ifElse.thenBlock.accept(*this);
  if (!ifElse.elseBlock.isEmpty())
    ifElse.elseBlock.accept(*this);
  return {};
}

any AstVisitor::visit(While &whileStmt) {
  whileStmt.condition->accept(*this);
  whileStmt.body.accept(*this);
  return {};
}

any AstVisitor::visit(ExpressionStatement &exprStmt) {
  exprStmt.expr->accept(*this);
  return {};
}

any AstVisitor::visit(VariableDeclaration &varDecl) {
  if (varDecl.initializer)
    varDecl.initializer->accept(*this);
  return {};
}

any AstVisitor::visit(StandaloneBlock &standaloneBlock) {
  standaloneBlock.block.accept(*this);
  return {};
}

// expressions
any AstVisitor::visit(Cast &cast) {
  cast.expr->accept(*this);
  return {};
}

any AstVisitor::visit(IntLiteral &intLit) { return {}; }

any AstVisitor::visit(FloatLiteral &floatLit) { return {}; }

any AstVisitor::visit(BoolLiteral &boolLit) { return {}; }

any AstVisitor::visit(StringLiteral &stringLit) { return {}; }

any AstVisitor::visit(VariableReference &var) { return {}; }

any AstVisitor::visit(ArrayReference &arr) {
  arr.index->accept(*this);
  return {};
}

any AstVisitor::visit(FunctionCall &funcCall) {
  for (auto &arg : funcCall.arguments) {
    arg->accept(*this);
  }
  return {};
}

any AstVisitor::visit(UnaryPrefixOp &unaryOp) {
  unaryOp.operand->accept(*this);
  return {};
}

any AstVisitor::visit(BinaryArithmetic &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(BinaryComparison &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(BinaryLogical &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(TernaryExpr &ternaryOp) {
  ternaryOp.condition->accept(*this);
  ternaryOp.thenExpr->accept(*this);
  ternaryOp.elseExpr->accept(*this);
  return {};
}

any AstVisitor::visit(Assignment &assignment) {
  assignment.target->accept(*this);
  assignment.value->accept(*this);
  return {};
}

// sugar
any AstVisitor::visit(sugar::ElifStatement &elifStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(sugar::IfElifElseStatement &elifElseStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(sugar::ForLoop &forStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(sugar::InIntervalExpr &inIntervalExpr) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(sugar::CompoundAssignment &compoundAssignment) {
  throw std::runtime_error("found sugar!");
}
