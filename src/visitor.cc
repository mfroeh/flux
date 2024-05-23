#include "visitor.hh"

#include "ast/ast.hh"
#include "ast/class.hh"
#include "ast/expr.hh"
#include "ast/module.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"

using namespace std;

// module
any AstVisitor::visit(Module &module) {
  for (auto &classDef : module.classes)
    classDef.accept(*this);

  for (auto &function : module.functions)
    function.accept(*this);

  return {};
}

// classes
any AstVisitor::visit(ClassDefinition &classDef) {
  for (auto &field : classDef.fields) {
    field.accept(*this);
  }
  for (auto &method : classDef.methods) {
    method.accept(*this);
  }
  return {};
}

any AstVisitor::visit(FieldDeclaration &field) { return {}; }

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

any AstVisitor::visit(Print &print) {
  for (auto &arg : print.args) {
    arg->accept(*this);
  }
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

any AstVisitor::visit(ArrayLiteral &arrInit) {
  for (auto &value : arrInit.values) {
    value->accept(*this);
  }
  return {};
}

any AstVisitor::visit(StructLiteral &structLit) {
  for (auto &[name, value] : structLit.fields) {
    value->accept(*this);
  }
  return {};
}

any AstVisitor::visit(VarRef &var) { return {}; }

any AstVisitor::visit(FieldRef &fieldRef) {
  fieldRef.object->accept(*this);
  return {};
}

any AstVisitor::visit(ArrayRef &arr) {
  arr.index->accept(*this);
  arr.arrayExpr->accept(*this);
  return {};
}

any AstVisitor::visit(FunctionCall &funcCall) {
  for (auto &arg : funcCall.args) {
    arg->accept(*this);
  }
  return {};
}

any AstVisitor::visit(MethodCall &methodCall) {
  methodCall.object->accept(*this);
  for (auto &arg : methodCall.args) {
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

any AstVisitor::visit(Pointer &pointer) {
  pointer.lvalue->accept(*this);
  return {};
}

any AstVisitor::visit(Dereference &dereference) {
  dereference.pointer->accept(*this);
  return {};
}

// sugar
any AstVisitor::visit(sugar::ElifStatement &elifStmt) {
  elifStmt.condition->accept(*this);
  elifStmt.thenBlock.accept(*this);
  return {};
}

any AstVisitor::visit(sugar::IfElifElseStatement &elifElseStmt) {
  elifElseStmt.condition->accept(*this);
  for (auto &elif : elifElseStmt.elseIfs) {
    elif->accept(*this);
  }
  elifElseStmt.thenBlock.accept(*this);
  return {};
}

any AstVisitor::visit(sugar::ForLoop &forStmt) {
  forStmt.initializer->accept(*this);
  forStmt.condition->accept(*this);
  forStmt.update->accept(*this);
  forStmt.body.accept(*this);
  return {};
}

any AstVisitor::visit(sugar::InIntervalExpr &inIntervalExpr) {
  inIntervalExpr.value->accept(*this);
  inIntervalExpr.lower->accept(*this);
  inIntervalExpr.upper->accept(*this);
  return {};
}

any AstVisitor::visit(sugar::CompoundAssignment &compoundAssignment) {
  compoundAssignment.value->accept(*this);
  compoundAssignment.target->accept(*this);
  return {};
}

any AstVisitor::visit(Halloc &halloc) {
  halloc.count->accept(*this);
  return {};
}