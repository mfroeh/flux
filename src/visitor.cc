#include "visitor.hh"

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"

// module
any AstVisitor::visit(struct Module &module) {
  for (auto &function : module.functions) {
    function.accept(*this);
  }
  return {};
}

// functions
any AstVisitor::visit(struct FunctionDefinition &function) {
  for (auto &parameter : function.parameters) {
    parameter.accept(*this);
  }
  function.body.accept(*this);
  return {};
}

any AstVisitor::visit(struct Parameter &parameter) { return {}; }

// statements
any AstVisitor::visit(struct Block &block) {
  for (auto &stmt : block.statements) {
    stmt->accept(*this);
  }
  return {};
}

any AstVisitor::visit(struct Return &ret) {
  ret.expression->accept(*this);
  return {};
}

any AstVisitor::visit(struct IfElse &ifElse) {
  ifElse.condition->accept(*this);
  ifElse.thenBlock.accept(*this);
  if (!ifElse.elseBlock.isEmpty())
    ifElse.elseBlock.accept(*this);
  return {};
}

any AstVisitor::visit(struct While &whileStmt) {
  whileStmt.condition->accept(*this);
  whileStmt.body.accept(*this);
  return {};
}

any AstVisitor::visit(struct ExpressionStatement &exprStmt) {
  exprStmt.expression->accept(*this);
  return {};
}

any AstVisitor::visit(struct VariableDeclaration &varDecl) {
  if (varDecl.initializer)
    varDecl.initializer->accept(*this);
  return {};
}

// expressions
any AstVisitor::visit(struct Cast &cast) {
  cast.expr->accept(*this);
  return {};
}

any AstVisitor::visit(struct IntLiteral &intLit) { return {}; }

any AstVisitor::visit(struct FloatLiteral &floatLit) { return {}; }

any AstVisitor::visit(struct BoolLiteral &boolLit) { return {}; }

any AstVisitor::visit(struct StringLiteral &stringLit) { return {}; }

any AstVisitor::visit(struct VariableReference &var) { return {}; }

any AstVisitor::visit(struct ArrayReference &arr) {
  arr.index->accept(*this);
  return {};
}

any AstVisitor::visit(struct FunctionCall &funcCall) {
  for (auto &arg : funcCall.arguments) {
    arg->accept(*this);
  }
  return {};
}

any AstVisitor::visit(struct UnaryPrefixOp &unaryOp) {
  unaryOp.operand->accept(*this);
  return {};
}

any AstVisitor::visit(struct BinaryArithmetic &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(struct BinaryComparison &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(struct BinaryLogical &binaryOp) {
  binaryOp.lhs->accept(*this);
  binaryOp.rhs->accept(*this);
  return {};
}

any AstVisitor::visit(struct TernaryExpr &ternaryOp) {
  ternaryOp.condition->accept(*this);
  ternaryOp.thenExpr->accept(*this);
  ternaryOp.elseExpr->accept(*this);
  return {};
}

any AstVisitor::visit(struct Assignment &assignment) {
  assignment.target->accept(*this);
  assignment.value->accept(*this);
  return {};
}

// sugar
any AstVisitor::visit(struct sugar::ElifStatement &elifStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(struct sugar::IfElifElseStatement &elifElseStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(struct sugar::ForLoop &forStmt) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(struct sugar::InIntervalExpr &inIntervalExpr) {
  throw std::runtime_error("found sugar!");
}

any AstVisitor::visit(struct sugar::CompoundAssignment &compoundAssignment) {
  throw std::runtime_error("found sugar!");
}
