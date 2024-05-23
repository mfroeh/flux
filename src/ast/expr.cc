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
  auto copy = make_shared<Cast>(tokens, expr->deepcopy(), type);
  copy->type = type;
  return copy;
}

llvm::Value *Cast::codegen(IRVisitor &visitor) { return visitor.visit(*this); }

IntLiteral::IntLiteral(Tokens tokens, long value)
    : Expr(std::move(tokens), IntType::get()), value(value) {}

any IntLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> IntLiteral::deepcopy() const {
  auto copy = make_shared<IntLiteral>(tokens, value);
  copy->type = type;
  return copy;
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
  auto copy = make_shared<FloatLiteral>(tokens, value);
  copy->type = type;
  return copy;
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
  auto copy = make_shared<BoolLiteral>(tokens, value);
  copy->type = type;
  return copy;
}

StringLiteral::StringLiteral(Tokens tokens, string value)
    : Expr(std::move(tokens), StringType::get()), value(value) {}

any StringLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> StringLiteral::deepcopy() const {
  auto copy = make_shared<StringLiteral>(tokens, value);
  copy->type = type;
  return copy;
}

llvm::Value *StringLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

ArrayLiteral::ArrayLiteral(Tokens tokens, vector<shared_ptr<Expr>> values)
    : Expr(std::move(tokens), InferType::get()), values(std::move(values)) {}

any ArrayLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *ArrayLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> ArrayLiteral::deepcopy() const {
  vector<shared_ptr<Expr>> newValues;
  for (auto &value : values) {
    newValues.push_back(value->deepcopy());
  }
  auto copy = make_shared<ArrayLiteral>(tokens, newValues);
  copy->type = type;
  return copy;
}

StructLiteral::StructLiteral(Tokens tokens, string name,
                             vector<string> fieldNames,
                             vector<shared_ptr<Expr>> values)
    : Expr(std::move(tokens), ClassType::get(name)), name(std::move(name)) {
  for (int i = 0; i < fieldNames.size(); i++) {
    fields.push_back({std::move(fieldNames[i]), std::move(values[i])});
  }
}

any StructLiteral::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *StructLiteral::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> StructLiteral::deepcopy() const {
  vector<string> fieldNames;
  vector<shared_ptr<Expr>> newValues;
  for (auto &[name, value] : fields) {
    fieldNames.push_back(name);
    newValues.push_back(value->deepcopy());
  }
  auto copy = make_shared<StructLiteral>(tokens, name, fieldNames, newValues);
  copy->type = type;
  return copy;
}

VarRef::VarRef(Tokens tokens, string name)
    : LValueExpr(std::move(tokens), InferType::get()), name(name) {}

any VarRef::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *VarRef::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> VarRef::deepcopy() const {
  auto copy = make_shared<VarRef>(tokens, name);
  copy->mangledName = mangledName;
  copy->type = type;
  return copy;
}

FieldRef::FieldRef(Tokens tokens, shared_ptr<Expr> object, string field)
    : LValueExpr(std::move(tokens), InferType::get()),
      object(std::move(object)), field(std::move(field)) {}

any FieldRef::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FieldRef::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> FieldRef::deepcopy() const {
  auto copy = make_shared<FieldRef>(tokens, object->deepcopy(), field);
  copy->type = type;
  return copy;
}

void FieldRef::setLhs(bool isLhs) {
  this->isLhs_ = isLhs;
  object->setLhs(isLhs);
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
  auto copy = make_shared<Pointer>(tokens, lvalue->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy = make_shared<Dereference>(tokens, pointer->deepcopy());
  copy->type = type;
  return copy;
}

void Dereference::setLhs(bool isLhs) {
  this->isLhs_ = isLhs;
  pointer->setLhs(isLhs);
}

ArrayRef::ArrayRef(Tokens tokens, shared_ptr<Expr> arrayExpr,
                   shared_ptr<Expr> index)
    : LValueExpr(std::move(tokens), InferType::get()),
      arrayExpr(std::move(arrayExpr)), index(std::move(index)) {}

any ArrayRef::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *ArrayRef::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

ostream &ArrayRef::print(ostream &out) const {
  out << *arrayExpr << "[" << *index << "]";
  return out;
}

shared_ptr<Expr> ArrayRef::deepcopy() const {
  auto copy =
      make_shared<ArrayRef>(tokens, arrayExpr->deepcopy(), index->deepcopy());
  copy->type = type;
  return copy;
}

void ArrayRef::setLhs(bool isLhs) {
  this->isLhs_ = isLhs;
  arrayExpr->setLhs(isLhs);
}

FunctionCall::FunctionCall(Tokens tokens, string callee,
                           vector<shared_ptr<Expr>> arguments)
    : Expr(std::move(tokens), InferType::get()), callee(std::move(callee)),
      args(std::move(arguments)) {}

any FunctionCall::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *FunctionCall::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> FunctionCall::deepcopy() const {
  vector<shared_ptr<Expr>> args;
  for (auto &arg : args) {
    args.push_back(arg->deepcopy());
  }
  auto copy = make_shared<FunctionCall>(tokens, callee, args);
  copy->mangledName = mangledName;
  copy->type = type;
  return copy;
}

MethodCall::MethodCall(Tokens tokens, shared_ptr<Expr> object, string callee,
                       vector<shared_ptr<Expr>> arguments)
    : FunctionCall(std::move(tokens), std::move(callee), std::move(arguments)),
      object(std::move(object)) {}

any MethodCall::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Value *MethodCall::codegen(IRVisitor &visitor) {
  assert(false && "should've been removed by typed desugarer");
}

shared_ptr<Expr> MethodCall::deepcopy() const {
  vector<shared_ptr<Expr>> args;
  for (auto &arg : args) {
    args.push_back(arg->deepcopy());
  }
  auto copy = make_shared<MethodCall>(tokens, object->deepcopy(), callee, args);
  copy->mangledName = mangledName;
  copy->type = type;
  return copy;
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
  auto copy = make_shared<UnaryPrefixOp>(tokens, op, operand->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy = make_shared<BinaryComparison>(tokens, lhs->deepcopy(), op,
                                            rhs->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy = make_shared<BinaryArithmetic>(tokens, lhs->deepcopy(), op,
                                            rhs->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy =
      make_shared<BinaryLogical>(tokens, lhs->deepcopy(), op, rhs->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy =
      make_shared<Assignment>(tokens, target->deepcopy(), value->deepcopy());
  copy->type = type;
  return copy;
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
  auto copy =
      make_shared<TernaryExpr>(tokens, condition->deepcopy(),
                               thenExpr->deepcopy(), elseExpr->deepcopy());
  copy->type = type;
  return copy;
}

VoidExpr::VoidExpr(Tokens tokens) : Expr(std::move(tokens), VoidType::get()) {}

any VoidExpr::accept(AbstractAstVisitor &visitor) { return {}; }

llvm::Value *VoidExpr::codegen(IRVisitor &visitor) { return nullptr; }

shared_ptr<Expr> VoidExpr::deepcopy() const {
  auto copy = make_shared<VoidExpr>(tokens);
  copy->type = type;
  return copy;
}

Halloc::Halloc(Tokens tokens, shared_ptr<Type> type, shared_ptr<Expr> count)
    : Expr(std::move(tokens), PointerType::get(type)),
      pointeeType(std::move(type)), count(std::move(count)) {}

any Halloc::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

llvm::Value *Halloc::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}

shared_ptr<Expr> Halloc::deepcopy() const {
  auto copy = make_shared<Halloc>(tokens, type, count->deepcopy());
  copy->type = type;
  return copy;
}