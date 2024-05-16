#include "ast/expr.hh"
#include "codegen/ir_visitor.hh"
#include "symbol.hh"
#include <llvm/ADT/APFloat.h>
#include <memory>

using namespace llvm;
using namespace std;

Value *IRVisitor::visit(Cast &cast) {
  auto value = cast.expr->codegen(*this);
  cout << "Cast: " << *cast.expr->type << " -> " << *cast.type << endl;
  return cast.expr->type->castTo(value, cast.type, *this);
}

Value *IRVisitor::visit(IntLiteral &literal) {
  return ConstantInt::get(*llvmContext, APInt(64, literal.value));
}

Value *IRVisitor::visit(FloatLiteral &literal) {
  // todo: check that this is double
  return ConstantFP::get(*llvmContext, APFloat(literal.value));
}

Value *IRVisitor::visit(BoolLiteral &literal) {
  return ConstantInt::get(*llvmContext, APInt(1, literal.value));
}

Value *IRVisitor::visit(::StringLiteral &literal) {
  throw runtime_error("Not implemented");
  // return builder->CreateGlobalStringPtr(literal.value);
}

Value *IRVisitor::visit(VariableReference &variable) {
  auto symbol = symTab.lookupVariable(variable.mangledName);
  assert(symbol);
  assert(symbol->type == variable.type);

  // always pass pointers instead of the array (also if array is rvalue)
  if (variable.isLValue() || variable.type->isArray())
    return symbol->alloc;

  return builder->CreateLoad(symbol->type->codegen(*this), symbol->alloc,
                             symbol->name);
}

Value *IRVisitor::visit(ArrayReference &ref) {
  auto index = ref.index->codegen(*this);
  auto value = ref.arrayExpr->codegen(*this);

  auto elemType = ref.type->codegen(*this);
  auto ptr = builder->CreateGEP(elemType, value, index, "arrayElemPtr");

  return ref.isLValue() ? ptr : builder->CreateLoad(elemType, ptr, "arrayElem");
}

Value *IRVisitor::visit(FunctionCall &call) {
  auto function = symTab.lookupFunction(call.mangledName);
  assert(function);

  vector<Value *> args;
  for (auto &arg : call.arguments) {
    args.push_back(arg->codegen(*this));
  }

  return builder->CreateCall(function->llvmFunction, args);
}

Value *IRVisitor::visit(UnaryPrefixOp &operation) {
  auto operand = operation.operand->codegen(*this);
  switch (operation.op) {
  case UnaryPrefixOp::Operator::Negate:
    return operation.operand->type->isInt() ? builder->CreateNeg(operand)
                                            : builder->CreateFNeg(operand);
  case UnaryPrefixOp::Operator::Not:
    return builder->CreateNot(operand);
  default:
    throw runtime_error("Unknown unary operator");
  }
}

Value *IRVisitor::visit(BinaryArithmetic &operation) {
  auto lhs = operation.lhs->codegen(*this);
  auto rhs = operation.rhs->codegen(*this);

  auto operandType = operation.lhs->type;
  switch (operation.op) {
  case BinaryArithmetic::Operator::Add:
    return operandType->isInt() ? builder->CreateAdd(lhs, rhs)
                                : builder->CreateFAdd(lhs, rhs);
  case BinaryArithmetic::Operator::Sub:
    return operandType->isInt() ? builder->CreateSub(lhs, rhs)
                                : builder->CreateFSub(lhs, rhs);
  case BinaryArithmetic::Operator::Mul:
    return operandType->isInt() ? builder->CreateMul(lhs, rhs)
                                : builder->CreateFMul(lhs, rhs);
  case BinaryArithmetic::Operator::Div:
    return operandType->isInt() ? builder->CreateSDiv(lhs, rhs)
                                : builder->CreateFDiv(lhs, rhs);
  case BinaryArithmetic::Operator::Mod:
    return operandType->isInt() ? builder->CreateSRem(lhs, rhs)
                                : builder->CreateFRem(lhs, rhs);
  default:
    throw runtime_error("Unknown binary operator");
  }
}

Value *IRVisitor::visit(BinaryComparison &operation) {
  auto lhs = operation.lhs->codegen(*this);
  auto rhs = operation.rhs->codegen(*this);

  auto operandType = operation.lhs->type;
  switch (operation.op) {
  case BinaryComparison::Operator::Eq:
    return operandType->isInt() ? builder->CreateICmpEQ(lhs, rhs)
                                : builder->CreateFCmpOEQ(lhs, rhs);
  case BinaryComparison::Operator::Ne:
    return operandType->isInt() ? builder->CreateICmpNE(lhs, rhs)
                                : builder->CreateFCmpONE(lhs, rhs);
  case BinaryComparison::Operator::Lt:
    return operandType->isInt() ? builder->CreateICmpSLT(lhs, rhs)
                                : builder->CreateFCmpOLT(lhs, rhs);
  case BinaryComparison::Operator::Le:
    return operandType->isInt() ? builder->CreateICmpSLE(lhs, rhs)
                                : builder->CreateFCmpOLE(lhs, rhs);
  case BinaryComparison::Operator::Gt:
    return operandType->isInt() ? builder->CreateICmpSGT(lhs, rhs)
                                : builder->CreateFCmpOGT(lhs, rhs);
  case BinaryComparison::Operator::Ge:
    return operandType->isInt() ? builder->CreateICmpSGE(lhs, rhs)
                                : builder->CreateFCmpOGE(lhs, rhs);
  default:
    throw runtime_error("Unknown binary operator");
  }
}

Value *IRVisitor::visit(BinaryLogical &operation) {
  auto lhs = operation.lhs->codegen(*this);
  auto rhs = operation.rhs->codegen(*this);

  switch (operation.op) {
  case BinaryLogical::Operator::And:
    return builder->CreateAnd(lhs, rhs);
  case BinaryLogical::Operator::Or:
    return builder->CreateOr(lhs, rhs);
  default:
    throw runtime_error("Unknown binary operator");
  }
}

Value *IRVisitor::visit(TernaryExpr &operation) {
  auto condition = operation.condition->codegen(*this);
  auto trueValue = operation.thenExpr->codegen(*this);
  auto falseValue = operation.elseExpr->codegen(*this);

  return builder->CreateSelect(condition, trueValue, falseValue);
}

Value *IRVisitor::visit(Assignment &assignment) {
  auto target = assignment.target->codegen(*this);
  auto value = assignment.value->codegen(*this);

  return builder->CreateStore(value, target, "assignment");
}