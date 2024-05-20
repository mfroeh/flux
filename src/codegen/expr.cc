#include "ast/expr.hh"
#include "ast/type.hh"
#include "codegen/ir_visitor.hh"
#include "symbol.hh"
#include <llvm/ADT/APFloat.h>
#include <memory>
#include <ranges>

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

Value *IRVisitor::visit(ArrayLiteral &literal) {
  auto arrType = literal.type->codegen(*this);
  auto alloca = builder->CreateAlloca(arrType, nullptr, "arrayLiteral");

  auto elemType = static_pointer_cast<::ArrayType>(literal.type)
                      ->elementType->codegen(*this);

  for (int i = 0; i < literal.values.size(); i++) {
    auto index = ConstantInt::get(*llvmContext, APInt(64, i));
    auto elemPtr = builder->CreateGEP(elemType, alloca, index, "arrayElemPtr");

    auto value = literal.values[i]->codegen(*this);
    builder->CreateStore(value, elemPtr);
  }

  // todo: should we also load here?
  return builder->CreateLoad(arrType, alloca, "arrayLiteral");
}

Value *IRVisitor::visit(StructLiteral &literal) {
  auto structType = literal.type->codegen(*this);
  auto alloca = builder->CreateAlloca(structType, nullptr, "structLiteral");

  auto classType = static_pointer_cast<ClassType>(literal.type);
  for (auto &[name, value] : literal.fields) {
    auto fieldPtr = classType->getFieldPtr(alloca, name, *this);
    auto fieldValue = value->codegen(*this);
    builder->CreateStore(fieldValue, fieldPtr);
  }

  return builder->CreateLoad(structType, alloca, "structLiteral");
}

Value *IRVisitor::visit(VarRef &variable) {
  auto symbol = symTab.lookupVariable(variable.mangledName);
  assert(symbol);

  // if this is a left-hand side of an assignment return address
  if (variable.isLhs())
    return symbol->alloc;

  return builder->CreateLoad(symbol->type->codegen(*this), symbol->alloc,
                             symbol->name);
}

Value *IRVisitor::visit(FieldRef &field) {
  // force to return address
  field.object->setLhs(true);
  auto obj = field.object->codegen(*this);
  assert(field.object->type->isClass());
  auto classType = static_pointer_cast<ClassType>(field.object->type);

  auto elemPtr = classType->getFieldPtr(obj, field.field, *this);
  return field.isLhs() ? elemPtr
                       : builder->CreateLoad(field.type->codegen(*this),
                                             elemPtr, "fieldRef");
}

// &x
Value *IRVisitor::visit(Pointer &pointer) {
  // force to return address
  pointer.lvalue->setLhs(true);
  return pointer.lvalue->codegen(*this);
}

// *(&x)
Value *IRVisitor::visit(Dereference &dereference) {
  // force to return address
  assert(dereference.pointer->type->isPointer());

  auto ptr = dereference.pointer->codegen(*this);
  auto derefType = dereference.type->codegen(*this);
  // lhs, load the address of the pointer, rhs, load the value of the pointer

  return dereference.isLhs()
             ? builder->CreateLoad(dereference.pointer->type->codegen(*this),
                                   ptr, "ptrAddress")
             : builder->CreateLoad(derefType, ptr, "deref");
}

Value *IRVisitor::visit(::ArrayRef &ref) {
  auto index = ref.index->codegen(*this);

  // force to return address
  ref.arrayExpr->setLhs(true);
  auto value = ref.arrayExpr->codegen(*this);

  auto elemType = ref.type->codegen(*this);
  auto ptr = builder->CreateGEP(elemType, value, index, "arrayElemPtr");

  return ref.isLhs() ? ptr : builder->CreateLoad(elemType, ptr, "arrayElem");
}

Value *IRVisitor::visit(FunctionCall &call) {
  auto function = symTab.lookupFunction(call.mangledName);
  assert(function);

  vector<Value *> args;
  for (auto &arg : call.args) {
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
  cout << "Binary arithmetic: " << *operation.lhs->type << " "
       << *operation.rhs->type << endl;
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

  builder->CreateStore(value, target, "storeAssign");
  return builder->CreateLoad(value->getType(), target, "loadAssigned");
}
