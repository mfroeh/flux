#include "ast/expr.hh"
#include "ast/type.hh"
#include "ir_visitor.hh"
#include "visitor.hh"

using namespace std;

Expr::Expr(Tokens tokens, shared_ptr<Type> type)
    : Node(std::move(tokens)), type(std::move(type)) {}

Cast::Cast(Tokens tokens, shared_ptr<Expr> expr, shared_ptr<Type> type)
    : Expr(std::move(tokens), std::move(type)), expr(std::move(expr)) {}

any Cast::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *Cast::codegen(IRVisitor &visitor) { return visitor.visit(*this); }

IntLiteral::IntLiteral(Tokens tokens, long value)
    : Expr(std::move(tokens), make_shared<IntType>()), value(value) {}

any IntLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *IntLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

FloatLiteral::FloatLiteral(Tokens tokens, double value)
    : Expr(std::move(tokens), make_shared<FloatType>()), value(value) {}

any FloatLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FloatLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BoolLiteral::BoolLiteral(Tokens tokens, bool value)
    : Expr(std::move(tokens), make_shared<BoolType>()), value(value) {}

any BoolLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BoolLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

StringLiteral::StringLiteral(Tokens tokens, string value)
    : Expr(std::move(tokens), make_shared<StringType>()), value(value) {}

any StringLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *StringLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

VariableReference::VariableReference(Tokens tokens, string name)
    : Expr(std::move(tokens), make_shared<InferType>()), name(name) {}

any VariableReference::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *VariableReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

ArrayReference::ArrayReference(Tokens tokens, shared_ptr<Expr> arrayExpr,
                               shared_ptr<Expr> index)
    : Expr(std::move(tokens), make_shared<InferType>()),
      arrayExpr(std::move(arrayExpr)), index(std::move(index)) {}

any ArrayReference::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *ArrayReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

FunctionCall::FunctionCall(Tokens tokens, string callee,
                           vector<shared_ptr<Expr>> arguments)
    : Expr(std::move(tokens), make_shared<InferType>()),
      callee(std::move(callee)), arguments(std::move(arguments)) {}

any FunctionCall::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FunctionCall::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

UnaryPrefixOp::UnaryPrefixOp(Tokens tokens, Operator op, shared_ptr<Expr> expr)
    : Expr(std::move(tokens), make_shared<InferType>()), op(op),
      operand(std::move(expr)) {}

any UnaryPrefixOp::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *UnaryPrefixOp::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryComparison::BinaryComparison(Tokens tokens, shared_ptr<Expr> lhs,
                                   Operator op, shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_shared<InferType>()), op(op),
      lhs(std::move(lhs)), rhs(std::move(rhs)) {}

any BinaryComparison::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryComparison::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryArithmetic::BinaryArithmetic(Tokens tokens, shared_ptr<Expr> lhs,
                                   Operator op, shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_shared<InferType>()), op(op),
      lhs(std::move(lhs)), rhs(std::move(rhs)) {}

any BinaryArithmetic::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryArithmetic::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

BinaryLogical::BinaryLogical(Tokens tokens, shared_ptr<Expr> lhs, Operator op,
                             shared_ptr<Expr> rhs)

    : Expr(std::move(tokens), make_shared<InferType>()), op(op),
      lhs(std::move(lhs)), rhs(std::move(rhs)) {}

any BinaryLogical::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryLogical::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

Assignment::Assignment(Tokens tokens, shared_ptr<Expr> lhs,
                       shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_shared<InferType>()), target(std::move(lhs)),
      value(std::move(rhs)) {}

any Assignment::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Assignment::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

TernaryExpr::TernaryExpr(Tokens tokens, shared_ptr<Expr> condition,
                         shared_ptr<Expr> thenExpr, shared_ptr<Expr> elseExpr)
    : Expr(std::move(tokens), make_shared<InferType>()),
      condition(std::move(condition)), thenExpr(std::move(thenExpr)),
      elseExpr(std::move(elseExpr)) {}

any TernaryExpr::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *TernaryExpr::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}
