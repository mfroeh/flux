#include "analysis/typecheck.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"
#include "ast/sugar.hh"
#include "ast/type.hh"
#include "symbol.hh"
#include "visitor.hh"
#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>

using namespace std;

// functions

stack<shared_ptr<FunctionSymbol>> functionSymbolStack;
stack<FunctionDefinition *> functionStack;

any TypeChecker::visit(FunctionDefinition &function) {
  auto symbol = symTab.lookupFunction(function.mangledName);
  functionSymbolStack.push(symbol);
  functionStack.push(&function);

  AstVisitor::visit(function);

  // if (function.returnType->isArray()) {
  //   throw runtime_error("Function cannot return an array");
  // }

  int returnCount = 0;
  for (auto &stmt : function.body.statements) {
    if (auto ret = dynamic_pointer_cast<Return>(stmt)) {
      returnCount++;
    }
  }

  if (returnCount == 0) {
    throw runtime_error("Function must have a top-level return statement");
  }

  functionSymbolStack.pop();
  functionStack.pop();

  return {};
}

any TypeChecker::visit(Parameter &param) {
  AstVisitor::visit(param);

  return {};
}

any TypeChecker::visit(Return &ret) {
  AstVisitor::visit(ret);

  auto &functionSymbol = functionSymbolStack.top();
  auto &currentFunction = functionStack.top();

  auto returnType = functionSymbol->returnType;
  if (returnType->isInfer()) {
    functionSymbol->returnType = ret.expression->type;
    currentFunction->returnType = ret.expression->type;
    return {};
  }

  if (!ret.expression->type->canImplicitlyConvertTo(returnType)) {
    throw runtime_error("Return type does not match function signature");
  } else if (ret.expression->type != returnType) {
    ret.expression = make_shared<Cast>(ret.expression, returnType);
  }

  return {};
}

// statements
any TypeChecker::visit(IfElse &ifElse) {
  AstVisitor::visit(ifElse);

  if (!ifElse.condition->type->isBool()) {
    throw runtime_error("If condition must be a boolean");
  }

  return {};
}

any TypeChecker::visit(While &whileStmt) {
  AstVisitor::visit(whileStmt);

  if (!whileStmt.condition->type->isBool()) {
    throw runtime_error("While condition must be a boolean");
  }

  return {};
}

any TypeChecker::visit(ExpressionStatement &exprStmt) {
  AstVisitor::visit(exprStmt);

  if (exprStmt.expr->type->isInfer()) {
    throw runtime_error("Expression type could not be inferred");
  }

  return {};
}

any TypeChecker::visit(VariableDeclaration &varDecl) {
  AstVisitor::visit(varDecl);

  if (varDecl.type->isVoid())
    throw runtime_error("Variable cannot have void type");

  if (varDecl.type->isInfer() && varDecl.initializer == nullptr)
    throw runtime_error("Variable type could not be inferred");

  if (varDecl.type->isInfer()) {
    varDecl.type = varDecl.initializer->type;
    auto symbol = symTab.lookupVariable(varDecl.mangledName);
    cout << varDecl.mangledName << endl;
    assert(symbol != nullptr);
    symbol->type = varDecl.type;
  }

  // if it was inferred to be void
  if (varDecl.type->isVoid())
    throw runtime_error("Variable cannot have void type");

  if (varDecl.initializer == nullptr && !varDecl.type->canDefaultInitialize()) {
    throw runtime_error("Variable must be initialized");
  }

  // if there is no initializer, and we can default initialize, no need to check
  // the type
  if (varDecl.initializer == nullptr)
    return {};

  if (!varDecl.initializer->type->canImplicitlyConvertTo(varDecl.type)) {
    throw runtime_error("Can not implicitly convert initializer to variable");
  } else if (varDecl.initializer->type != varDecl.type) {
    cout << "Casting:" << *varDecl.initializer->type << " -> " << *varDecl.type
         << endl;
    varDecl.initializer = make_shared<Cast>(varDecl.initializer, varDecl.type);
  }

  return {};
}

// expressions
any TypeChecker::visit(Cast &cast) {
  AstVisitor::visit(cast);

  if (!cast.expr->type->canImplicitlyConvertTo(cast.type)) {
    ostringstream oss;
    oss << "Cannot cast from " << cast.expr->type << " to " << cast.type;
    throw runtime_error(oss.str());
  }

  return {};
}

any TypeChecker::visit(IntLiteral &intLit) {
  intLit.type = IntType::get();
  return {};
}

any TypeChecker::visit(FloatLiteral &floatLit) {
  floatLit.type = FloatType::get();
  return {};
}

any TypeChecker::visit(BoolLiteral &boolLit) {
  boolLit.type = BoolType::get();
  return {};
}

any TypeChecker::visit(StringLiteral &stringLit) {
  stringLit.type = StringType::get();
  return {};
}

any TypeChecker::visit(ArrayLiteral &arrLiteral) {
  AstVisitor::visit(arrLiteral);

  // the first element decides the element type
  auto elemType = arrLiteral.values[0]->type;

  for (auto &value : arrLiteral.values) {
    if (!value->type->canImplicitlyConvertTo(elemType)) {
      throw runtime_error(
          "Array initializer value does not match element type");
    } else if (value->type != elemType) {
      value = make_shared<Cast>(value, elemType);
    }
  }

  arrLiteral.type = ArrayType::get(elemType, arrLiteral.values.size());

  return {};
}

any TypeChecker::visit(VariableReference &var) {
  auto symbol = symTab.lookupVariable(var.mangledName);
  assert(symbol != nullptr);
  var.type = symbol->type;
  assert(!var.type->isInfer());
  return {};
}

any TypeChecker::visit(ArrayReference &arr) {
  AstVisitor::visit(arr);

  if (!arr.index->type->isInt()) {
    throw runtime_error("Array index must be an integer");
  }

  if (!arr.arrayExpr->type->isArray()) {
    cout << *arr.arrayExpr->type << endl;
    throw runtime_error("Array reference must be an array");
  }

  arr.type = static_pointer_cast<ArrayType>(arr.arrayExpr->type)->elementType;
  return {};
}

any TypeChecker::visit(FunctionCall &funcCall) {
  AstVisitor::visit(funcCall);

  vector<string> remainingCandidates;

  // candidates are all visible functions with a matching name
  // now we resolve the candidates by checking the argument types
  for (auto &candidate : funcCall.callCandidates) {
    auto functionSymbol = symTab.lookupFunction(candidate);
    if (functionSymbol->parameters.size() != funcCall.arguments.size()) {
      continue;
    }

    bool match = true;
    for (int i = 0; i < functionSymbol->parameters.size(); i++) {
      auto &param = functionSymbol->parameters[i];
      auto &arg = funcCall.arguments[i];
      if (!arg->type->canImplicitlyConvertTo(param.type)) {
        match = false;
        break;
      }
    }

    if (match)
      remainingCandidates.push_back(candidate);
  }

  if (remainingCandidates.size() == 0) {
    ostringstream oss;
    oss << "No matching function found for call to " << funcCall.callee;
    throw runtime_error(oss.str());
  }

  vector<shared_ptr<FunctionSymbol>> candidates;
  for (auto &candidate : remainingCandidates) {
    candidates.push_back(symTab.lookupFunction(candidate));
  }

  // favor by scope depth, then by definition depth
  // TODO: we could also make it illegal to have multiple possible functions in
  // the same scope
  ranges::sort(candidates, [](auto &a, auto &b) {
    if (a->depth > b->depth)
      return true;
    return a->count > b->count;
  });

  auto symbol = candidates[0];
  for (int i = 0; i < funcCall.arguments.size(); i++) {
    auto &arg = funcCall.arguments[i];
    auto &param = symbol->parameters[i];
    if (arg->type != param.type) {
      funcCall.arguments[i] = make_shared<Cast>(arg, param.type);
    }
  }

  funcCall.mangledName = remainingCandidates[0];
  funcCall.type = symbol->returnType;
  return {};
}

any TypeChecker::visit(UnaryPrefixOp &unaryOp) {
  AstVisitor::visit(unaryOp);

  if (unaryOp.op == UnaryPrefixOp::Operator::Negate) {
    if (!unaryOp.operand->type->isNumber()) {
      throw runtime_error("Unary negate operator must be applied to a number");
    }
    unaryOp.type = unaryOp.operand->type;
  } else if (unaryOp.op == UnaryPrefixOp::Operator::Not) {
    if (!unaryOp.operand->type->canImplicitlyConvertTo(BoolType::get())) {
      throw runtime_error("Unary not operator must be applied to a boolean");
    } else if (unaryOp.operand->type != BoolType::get()) {
      unaryOp.operand = make_shared<Cast>(unaryOp.operand, BoolType::get());
    }
    unaryOp.type = BoolType::get();
  } else {
    throw runtime_error("Unknown unary operator");
  }

  return {};
}

any TypeChecker::visit(BinaryArithmetic &binaryOp) {
  AstVisitor::visit(binaryOp);

  if (!binaryOp.lhs->type->isNumber() || !binaryOp.rhs->type->isNumber()) {
    throw runtime_error(
        "Binary arithmetic operator must be applied to numbers");
  }

  auto lhsType = binaryOp.lhs->type;
  auto rhsType = binaryOp.rhs->type;
  if (lhsType != rhsType) {
    if (lhsType->canImplicitlyConvertTo(rhsType)) {
      binaryOp.lhs = make_shared<Cast>(binaryOp.lhs, rhsType);
    } else if (rhsType->canImplicitlyConvertTo(lhsType)) {
      binaryOp.rhs = make_shared<Cast>(binaryOp.rhs, lhsType);
    } else {
      throw runtime_error("Binary arithmetic operator types do not match");
    }
  }

  binaryOp.type = binaryOp.lhs->type;

  return {};
}

any TypeChecker::visit(BinaryComparison &binaryOp) {
  AstVisitor::visit(binaryOp);

  if (!binaryOp.lhs->type->isNumber() || !binaryOp.rhs->type->isNumber()) {
    throw runtime_error(
        "Binary comparison operator must be applied to numbers");
  }

  auto lhsType = binaryOp.lhs->type;
  auto rhsType = binaryOp.rhs->type;
  if (lhsType != rhsType) {
    if (lhsType->canImplicitlyConvertTo(rhsType)) {
      binaryOp.lhs = make_shared<Cast>(binaryOp.lhs, rhsType);
    } else if (rhsType->canImplicitlyConvertTo(lhsType)) {
      binaryOp.rhs = make_shared<Cast>(binaryOp.rhs, lhsType);
    } else {
      throw runtime_error("Binary comparison operator types do not match");
    }
  }

  binaryOp.type = BoolType::get();

  return {};
}

any TypeChecker::visit(BinaryLogical &binaryOp) {
  AstVisitor::visit(binaryOp);

  if (!binaryOp.lhs->type->canImplicitlyConvertTo(BoolType::get()) ||
      !binaryOp.lhs->type->canImplicitlyConvertTo(BoolType::get())) {
    throw runtime_error("Binary logical operator must be applied to booleans");
  } else {
    if (binaryOp.lhs->type != BoolType::get()) {
      binaryOp.lhs = make_shared<Cast>(binaryOp.lhs, BoolType::get());
    }

    if (binaryOp.rhs->type != BoolType::get()) {
      binaryOp.rhs = make_shared<Cast>(binaryOp.rhs, BoolType::get());
    }
  }

  binaryOp.type = BoolType::get();

  return {};
}

any TypeChecker::visit(Assignment &assignment) {
  AstVisitor::visit(assignment);

  assert(assignment.target->isLhs());

  if (!assignment.target->isLValue()) {
    throw runtime_error("Can only assign to lvalues");
  }

  assert(!assignment.target->type->isInfer());

  if (!assignment.value->type->canImplicitlyConvertTo(
          assignment.target->type)) {
    throw runtime_error("Cannot assign value to target of different type");
  } else if (assignment.value->type != assignment.target->type) {
    assignment.value =
        make_shared<Cast>(assignment.value, assignment.target->type);
  }

  assignment.type = assignment.target->type;

  return {};
}

any TypeChecker::visit(TernaryExpr &ternaryOp) {
  AstVisitor::visit(ternaryOp);

  if (!ternaryOp.condition->type->canImplicitlyConvertTo(BoolType::get())) {
    throw runtime_error("Ternary condition must be a boolean");
  } else if (ternaryOp.condition->type != BoolType::get()) {
    ternaryOp.condition =
        make_shared<Cast>(ternaryOp.condition, BoolType::get());
  }

  auto trueType = ternaryOp.thenExpr->type;
  auto falseType = ternaryOp.elseExpr->type;

  if (!trueType->canImplicitlyConvertTo(falseType) &&
      !falseType->canImplicitlyConvertTo(trueType)) {
    throw runtime_error("Ternary expressions must have compatible types");
  } else if (trueType != falseType) {
    if (trueType->canImplicitlyConvertTo(falseType)) {
      ternaryOp.thenExpr = make_shared<Cast>(ternaryOp.thenExpr, falseType);
    } else {
      ternaryOp.elseExpr = make_shared<Cast>(ternaryOp.elseExpr, trueType);
    }
  }

  ternaryOp.type = ternaryOp.thenExpr->type;

  return {};
}

any TypeChecker::visit(Pointer &pointer) {
  AstVisitor::visit(pointer);

  if (!pointer.lvalue->isLValue()) {
    throw runtime_error("Can only create a pointer to an lvalue");
  }

  pointer.type = PointerType::get(pointer.lvalue->type);
  return {};
}

any TypeChecker::visit(Dereference &dereference) {
  AstVisitor::visit(dereference);

  if (!dereference.pointer->type->isPointer()) {
    throw runtime_error("Dereference operator must be applied to a pointer");
  }

  dereference.type =
      static_pointer_cast<PointerType>(dereference.pointer->type)->pointee;

  return {};
}

any TypeChecker::visit(sugar::ElifStatement &elifStmt) {
  AstVisitor::visit(elifStmt);

  if (!elifStmt.condition->type->canImplicitlyConvertTo(BoolType::get())) {
    throw runtime_error("Elif condition must be a boolean");
  }

  return {};
}

any TypeChecker::visit(sugar::IfElifElseStatement &elifElseStmt) {
  AstVisitor::visit(elifElseStmt);

  if (!elifElseStmt.condition->type->canImplicitlyConvertTo(BoolType::get())) {
    throw runtime_error("If condition must be a boolean");
  }

  return {};
}

any TypeChecker::visit(sugar::ForLoop &forStmt) {
  AstVisitor::visit(forStmt);

  if (!forStmt.condition->type->canImplicitlyConvertTo(BoolType::get())) {
    throw runtime_error("For loop condition must be a boolean");
  } else if (forStmt.condition->type != BoolType::get()) {
    forStmt.condition = make_shared<Cast>(forStmt.condition, BoolType::get());
  }

  return {};
}

any TypeChecker::visit(sugar::InIntervalExpr &inIntervalExpr) {
  AstVisitor::visit(inIntervalExpr);

  if (!inIntervalExpr.value->type->isNumber()) {
    throw runtime_error("In interval expression must be a number");
  }

  if (inIntervalExpr.upper->type != inIntervalExpr.lower->type) {
    throw runtime_error("In interval bounds must have the same type");
  }

  // convert value to bounds type
  if (!inIntervalExpr.value->type->canImplicitlyConvertTo(
          inIntervalExpr.upper->type)) {
    throw runtime_error("In interval value must match interval type");
  } else if (inIntervalExpr.value->type != inIntervalExpr.upper->type) {
    inIntervalExpr.value =
        make_shared<Cast>(inIntervalExpr.value, inIntervalExpr.upper->type);
  }

  inIntervalExpr.type = BoolType::get();

  return {};
}

any TypeChecker::visit(sugar::CompoundAssignment &compoundAssignment) {
  AstVisitor::visit(compoundAssignment);

  if (!compoundAssignment.target->isLValue()) {
    throw runtime_error("Compound assignment target must be an lvalue");
  }

  if (!compoundAssignment.value->type->canImplicitlyConvertTo(
          compoundAssignment.value->type)) {
    throw runtime_error("Compound assignment value must match target type");
  } else if (compoundAssignment.value->type !=
             compoundAssignment.target->type) {
    compoundAssignment.value = make_shared<Cast>(
        compoundAssignment.value, compoundAssignment.target->type);
  }

  compoundAssignment.type = compoundAssignment.target->type;

  return {};
}