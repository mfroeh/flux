#include "ast/expr.hh"
#include "ast/type.hh"
#include "codegen/ir_visitor.hh"
#include "visitor.hh"
#include <memory>

using namespace std;

Expr::Expr(Tokens tokens, shared_ptr<Type> type)
    : Node(std::move(tokens)), type(std::move(type)) {}

void Expr::setLhs(bool isLhs) { this->isLhs_ = isLhs; };

LValueExpr::LValueExpr(Tokens tokens, shared_ptr<Type> type)
    : Expr(std::move(tokens), std::move(type)) {}

Cast::Cast(Tokens tokens, shared_ptr<Expr> expr, shared_ptr<Type> type)
    : Expr(std::move(tokens), std::move(type)), expr(std::move(expr)) {}

Cast::Cast(shared_ptr<Expr> expr, shared_ptr<Type> type)
    : Expr(expr->tokens, std::move(type)), expr(std::move(expr)) {}

any Cast::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

shared_ptr<Expr> Cast::deepcopy() const {
  return make_shared<Cast>(tokens, expr->deepcopy(), type);
}

llvm::Value *Cast::codegen(IRVisitor &visitor) { return visitor.visit(*this); }

IntLiteral::IntLiteral(Tokens tokens, long value)
    : Expr(std::move(tokens), IntType::get()), value(value) {}

any IntLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> IntLiteral::deepcopy() const {
  return make_shared<IntLiteral>(tokens, value);
}

llvm::Value *IntLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

FloatLiteral::FloatLiteral(Tokens tokens, double value)
    : Expr(std::move(tokens), FloatType::get()), value(value) {}

any FloatLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FloatLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> FloatLiteral::deepcopy() const {
  return make_shared<FloatLiteral>(tokens, value);
}

BoolLiteral::BoolLiteral(Tokens tokens, bool value)
    : Expr(std::move(tokens), BoolType::get()), value(value) {}

any BoolLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BoolLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> BoolLiteral::deepcopy() const {
  return make_shared<BoolLiteral>(tokens, value);
}

StringLiteral::StringLiteral(Tokens tokens, string value)
    : Expr(std::move(tokens), StringType::get()), value(value) {}

any StringLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> StringLiteral::deepcopy() const {
  return make_shared<StringLiteral>(tokens, value);
}

llvm::Value *StringLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

VariableReference::VariableReference(Tokens tokens, string name)
    : LValueExpr(std::move(tokens), InferType::get()), name(name) {}

any VariableReference::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *VariableReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> VariableReference::deepcopy() const {
  return make_shared<VariableReference>(tokens, name);
}

Pointer::Pointer(Tokens tokens, shared_ptr<Expr> lvalue)
    : Expr(std::move(tokens), InferType::get()), lvalue(std::move(lvalue)) {}

any Pointer::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Pointer::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> Pointer::deepcopy() const {
  return make_shared<Pointer>(tokens, lvalue->deepcopy());
}

Dereference::Dereference(Tokens tokens, shared_ptr<Expr> expr)
    : LValueExpr(std::move(tokens), InferType::get()),
      pointer(std::move(expr)) {}

any Dereference::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Dereference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> Dereference::deepcopy() const {
  return make_shared<Dereference>(tokens, pointer->deepcopy());
}

void Dereference::setLhs(bool isLhs) {
  cout << "dereference setLhs with " << isLhs << endl;
  this->isLhs_ = isLhs;
  pointer->setLhs(isLhs);
}

ArrayReference::ArrayReference(Tokens tokens, shared_ptr<Expr> arrayExpr,
                               shared_ptr<Expr> index)
    : LValueExpr(std::move(tokens), InferType::get()),
      arrayExpr(std::move(arrayExpr)), index(std::move(index)) {}

any ArrayReference::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *ArrayReference::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> ArrayReference::deepcopy() const {
  return make_shared<ArrayReference>(tokens, arrayExpr->deepcopy(),
                                     index->deepcopy());
}

void ArrayReference::setLhs(bool isLhs) {
  this->isLhs_ = isLhs;
  arrayExpr->setLhs(isLhs);
}

FunctionCall::FunctionCall(Tokens tokens, string callee,
                           vector<shared_ptr<Expr>> arguments)
    : Expr(std::move(tokens), InferType::get()), callee(std::move(callee)),
      arguments(std::move(arguments)) {}

any FunctionCall::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FunctionCall::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> FunctionCall::deepcopy() const {
  vector<shared_ptr<Expr>> args;
  for (auto &arg : arguments) {
    args.push_back(arg->deepcopy());
  }
  return make_shared<FunctionCall>(tokens, callee, args);
}

UnaryPrefixOp::UnaryPrefixOp(Tokens tokens, Operator op, shared_ptr<Expr> expr)
    : Expr(std::move(tokens), InferType::get()), op(op),
      operand(std::move(expr)) {}

any UnaryPrefixOp::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *UnaryPrefixOp::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> UnaryPrefixOp::deepcopy() const {
  return make_shared<UnaryPrefixOp>(tokens, op, operand->deepcopy());
}

BinaryComparison::BinaryComparison(Tokens tokens, shared_ptr<Expr> lhs,
                                   Operator op, shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), InferType::get()), op(op), lhs(std::move(lhs)),
      rhs(std::move(rhs)) {}

any BinaryComparison::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryComparison::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> BinaryComparison::deepcopy() const {
  return make_shared<BinaryComparison>(tokens, lhs->deepcopy(), op,
                                       rhs->deepcopy());
}

BinaryArithmetic::BinaryArithmetic(Tokens tokens, shared_ptr<Expr> lhs,
                                   Operator op, shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), InferType::get()), op(op), lhs(std::move(lhs)),
      rhs(std::move(rhs)) {}

any BinaryArithmetic::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryArithmetic::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> BinaryArithmetic::deepcopy() const {
  return make_shared<BinaryArithmetic>(tokens, lhs->deepcopy(), op,
                                       rhs->deepcopy());
}

BinaryLogical::BinaryLogical(Tokens tokens, shared_ptr<Expr> lhs, Operator op,
                             shared_ptr<Expr> rhs)

    : Expr(std::move(tokens), InferType::get()), op(op), lhs(std::move(lhs)),
      rhs(std::move(rhs)) {}

any BinaryLogical::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryLogical::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> BinaryLogical::deepcopy() const {
  return make_shared<BinaryLogical>(tokens, lhs->deepcopy(), op,
                                    rhs->deepcopy());
}

Assignment::Assignment(Tokens tokens, shared_ptr<Expr> lhs,
                       shared_ptr<Expr> rhs)
    : Expr(std::move(tokens), InferType::get()), target(std::move(lhs)),
      value(std::move(rhs)) {}

any Assignment::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Assignment::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> Assignment::deepcopy() const {
  return make_shared<Assignment>(tokens, target->deepcopy(), value->deepcopy());
}

TernaryExpr::TernaryExpr(Tokens tokens, shared_ptr<Expr> condition,
                         shared_ptr<Expr> thenExpr, shared_ptr<Expr> elseExpr)
    : Expr(std::move(tokens), InferType::get()),
      condition(std::move(condition)), thenExpr(std::move(thenExpr)),
      elseExpr(std::move(elseExpr)) {}

any TernaryExpr::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *TernaryExpr::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> TernaryExpr::deepcopy() const {
  return make_shared<TernaryExpr>(tokens, condition->deepcopy(),
                                  thenExpr->deepcopy(), elseExpr->deepcopy());
}