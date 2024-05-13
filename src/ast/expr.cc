#include "ast/expr.hh"
#include "ast/type.hh"
#include "ir_visitor.hh"
#include "visitor.hh"

using namespace std;

Expr::Expr(Tokens tokens, unique_ptr<Type> type)
    : Node(std::move(tokens)), type(std::move(type)) {}

Cast::Cast(Tokens tokens, unique_ptr<Expr> expr, unique_ptr<Type> type)
    : Expr(std::move(tokens), std::move(type)), expr(std::move(expr)) {}

any Cast::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *Cast::codegen(IRVisitor &visitor) { return visitor.visit(*this); }

IntLiteral::IntLiteral(Tokens tokens, long value)
    : Expr(std::move(tokens), make_unique<IntType>()), value(value) {}

any IntLiteral::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *IntLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

FloatLiteral::FloatLiteral(Tokens tokens, double value)
    : Expr(std::move(tokens), make_unique<FloatType>()), value(value) {}

any FloatLiteral::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *FloatLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BoolLiteral::BoolLiteral(Tokens tokens, bool value)
    : Expr(std::move(tokens), make_unique<BoolType>()), value(value) {}

any BoolLiteral::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *BoolLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

StringLiteral::StringLiteral(Tokens tokens, string value)
    : Expr(std::move(tokens), make_unique<StringType>()), value(value) {}

any StringLiteral::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *StringLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

VariableReference::VariableReference(Tokens tokens, string name)
    : Expr(std::move(tokens), make_unique<InferType>()), name(name) {}

any VariableReference::accept(AstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *VariableReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

ArrayReference::ArrayReference(Tokens tokens, unique_ptr<Expr> array,
                               unique_ptr<Expr> index)
    : Expr(std::move(tokens), make_unique<InferType>()),
      array(std::move(array)), index(std::move(index)) {}

any ArrayReference::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *ArrayReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

FunctionCall::FunctionCall(Tokens tokens, string callee,
                           vector<unique_ptr<Expr>> arguments)
    : Expr(std::move(tokens), make_unique<InferType>()),
      callee(std::move(callee)), arguments(std::move(arguments)) {}

any FunctionCall::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *FunctionCall::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

UnaryPrefixOp::UnaryPrefixOp(Tokens tokens, Operator op, unique_ptr<Expr> expr)
    : Expr(std::move(tokens), make_unique<InferType>()), op(op),
      operand(std::move(expr)) {}

any UnaryPrefixOp::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *UnaryPrefixOp::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryComparison::BinaryComparison(Tokens tokens, unique_ptr<Expr> lhs,
                                   Operator op, unique_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_unique<InferType>()), op(op),
      left(std::move(lhs)), right(std::move(rhs)) {}

any BinaryComparison::accept(AstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryComparison::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryArithmetic::BinaryArithmetic(Tokens tokens, unique_ptr<Expr> lhs,
                                   Operator op, unique_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_unique<InferType>()), op(op),
      left(std::move(lhs)), right(std::move(rhs)) {}

any BinaryArithmetic::accept(AstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryArithmetic::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryLogical::BinaryLogical(Tokens tokens, unique_ptr<Expr> lhs, Operator op,
                             unique_ptr<Expr> rhs)

    : Expr(std::move(tokens), make_unique<InferType>()), op(op),
      left(std::move(lhs)), right(std::move(rhs)) {}

any BinaryLogical::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *BinaryLogical::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

Assignment::Assignment(Tokens tokens, string lhs, unique_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_unique<InferType>()), target(std::move(lhs)),
      value(std::move(rhs)) {}

any Assignment::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *Assignment::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

ArrayAssignment::ArrayAssignment(Tokens tokens, string array,
                                 unique_ptr<Expr> index, unique_ptr<Expr> value)
    : Expr(std::move(tokens), make_unique<InferType>()),
      array(std::move(array)), index(std::move(index)),
      value(std::move(value)) {}

any ArrayAssignment::accept(AstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *ArrayAssignment::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

TernaryExpr::TernaryExpr(Tokens tokens, unique_ptr<Expr> condition,
                         unique_ptr<Expr> thenExpr, unique_ptr<Expr> elseExpr)
    : Expr(std::move(tokens), make_unique<InferType>()),
      condition(std::move(condition)), thenExpr(std::move(thenExpr)),
      elseExpr(std::move(elseExpr)) {}

any TernaryExpr::accept(AstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *TernaryExpr::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}
