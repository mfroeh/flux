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
    : Expr(std::move(tokens), InferType::get()), value(std::move(value)),
      lower(std::move(lower)), upper(std::move(upper)), kind(intervalKind) {}

any InIntervalExpr::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> InIntervalExpr::deepcopy() const {
  auto copy = make_shared<InIntervalExpr>(
      tokens, value->deepcopy(), lower->deepcopy(), upper->deepcopy(), kind);
  copy->type = type;
  return copy;
}

CompoundAssignment::CompoundAssignment(Tokens tokens, shared_ptr<Expr> lhs,
                                       BinaryArithmetic::Operator op,
                                       shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), InferType::get()), target(std::move(lhs)),
      value(std::move(rhs)), op(op) {}

any CompoundAssignment::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> CompoundAssignment::deepcopy() const {
  auto copy = make_shared<CompoundAssignment>(tokens, target->deepcopy(), op,
                                              value->deepcopy());
  copy->type = type;
  return copy;
}