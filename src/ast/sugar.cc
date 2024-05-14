#include "ast/sugar.hh"
#include "ast/expr.hh"
#include "ast/type.hh"
#include "visitor.hh"

using namespace sugar;
using namespace std;

ElifStatement::ElifStatement(Tokens tokens, shared_ptr<Expr> condition,
                             Block thenBlock)
    : Statement(std::move(tokens)), condition(std::move(condition)),
      thenBlock(std::move(thenBlock)) {}

any ElifStatement::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

IfElifElseStatement::IfElifElseStatement(
    Tokens tokens, shared_ptr<Expr> condition, Block thenBlock,
    vector<shared_ptr<ElifStatement>> elifs, Block elseBlock)
    : Statement(std::move(tokens)), condition(std::move(condition)),
      thenBlock(std::move(thenBlock)), elseIfs(std::move(elifs)),
      elseBlock(std::move(elseBlock)) {}

any IfElifElseStatement::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

ForLoop::ForLoop(Tokens tokens, shared_ptr<Statement> initializer,
                 shared_ptr<Expr> condition, shared_ptr<Statement> update,
                 Block body)
    : Statement(std::move(tokens)), initializer(std::move(initializer)),
      condition(std::move(condition)), update(std::move(update)),
      body(std::move(body)) {}

any ForLoop::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

InIntervalExpr::InIntervalExpr(Tokens tokens, shared_ptr<Expr> value,
                               shared_ptr<Expr> lower, shared_ptr<Expr> upper,
                               IntervalKind intervalKind)
    : Expr(std::move(tokens), make_shared<InferType>()),
      value(std::move(value)), lower(std::move(lower)), upper(std::move(upper)),
      kind(intervalKind) {}

any InIntervalExpr::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *InIntervalExpr::codegen(IRVisitor &visitor) {
  throw runtime_error("InIntervalExpr should not exist during codegen");
}

CompoundAssignment::CompoundAssignment(Tokens tokens, shared_ptr<Expr> lhs,
                                       BinaryArithmetic::Operator op,
                                       shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), make_shared<InferType>()), target(std::move(lhs)),
      value(std::move(rhs)), op(op) {}

any CompoundAssignment::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *CompoundAssignment::codegen(IRVisitor &visitor) {
  throw runtime_error("CompoundAssignment should not exist during codegen");
}