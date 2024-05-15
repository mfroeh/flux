#include "analysis/typecheck.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"
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
stack<int> returns;

any TypeChecker::visit(FunctionDefinition &function) {
  auto symbol = symTab.lookupFunction(function.mangledName);
  functionSymbolStack.push(symbol);
  functionStack.push(&function);
  returns.push(0);

  AstVisitor::visit(function);

  if (returns.top() == 0 && !function.returnType->isVoid()) {
    throw runtime_error("Function must return a value");
  }

  functionSymbolStack.pop();
  functionStack.pop();
  returns.pop();

  return {};
}

any TypeChecker::visit(Return &ret) {
  AstVisitor::visit(ret);

  returns.top()++;

  auto &functionSymbol = functionSymbolStack.top();
  auto &currentFunction = functionStack.top();

  auto returnType = functionSymbol->returnType;
  if (returnType->isInfer()) {
    functionSymbol->returnType = ret.expression->type;
    currentFunction->returnType = ret.expression->type;
    return {};
  }

  if (!ret.expression->type->canImplicitlyConvert(returnType)) {
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

  if (varDecl.initializer == nullptr)
    return {};

  if (varDecl.type->isInfer()) {
    varDecl.type = varDecl.initializer->type;
    auto symbol = symTab.lookupVariable(varDecl.mangledName);
    cout << varDecl.mangledName << endl;
    assert(symbol != nullptr);
    symbol->type = varDecl.type;
  }

  if (!varDecl.initializer->type->canImplicitlyConvert(varDecl.type)) {
    throw runtime_error("Variable type does not match initializer type");
  } else if (varDecl.initializer->type != varDecl.type) {
    varDecl.initializer = make_shared<Cast>(varDecl.initializer, varDecl.type);
  }

  return {};
}

// expressions
any TypeChecker::visit(Cast &cast) {
  AstVisitor::visit(cast);

  if (!cast.expr->type->canImplicitlyConvert(cast.type)) {
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

any TypeChecker::visit(VariableReference &var) {
  auto symbol = symTab.lookupVariable(var.name);
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
      if (!arg->type->canImplicitlyConvert(param.type)) {
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
  } else if (remainingCandidates.size() > 1) {
    ostringstream oss;
    oss << "Ambiguous call to function " << funcCall.callee
        << " with candidates: ";
    for (auto &candidate : remainingCandidates) {
      oss << candidate << " ";
    }
    throw runtime_error(oss.str());
  }

  auto symbol = symTab.lookupFunction(remainingCandidates[0]);
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
    if (!unaryOp.operand->type->canImplicitlyConvert(BoolType::get())) {
      throw runtime_error("Unary not operator must be applied to a boolean");
    } else if (unaryOp.operand->type != BoolType::get()) {
      unaryOp.operand = make_shared<Cast>(unaryOp.operand, BoolType::get());
    }
    unaryOp.type = BoolType::get();
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
    if (lhsType->canImplicitlyConvert(rhsType)) {
      binaryOp.lhs = make_shared<Cast>(binaryOp.lhs, rhsType);
    } else if (rhsType->canImplicitlyConvert(lhsType)) {
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
    if (lhsType->canImplicitlyConvert(rhsType)) {
      binaryOp.lhs = make_shared<Cast>(binaryOp.lhs, rhsType);
    } else if (rhsType->canImplicitlyConvert(lhsType)) {
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

  if (!binaryOp.lhs->type->canImplicitlyConvert(BoolType::get()) ||
      !binaryOp.lhs->type->canImplicitlyConvert(BoolType::get())) {
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

  bool isVariable =
      dynamic_pointer_cast<VariableReference>(assignment.target) != nullptr;
  bool isArrayReference =
      dynamic_pointer_cast<ArrayReference>(assignment.target) != nullptr;

  if (!isVariable && !isArrayReference) {
    throw runtime_error("Can only assign to variables or array indices");
  }

  assert(!assignment.target->type->isInfer());

  if (!assignment.value->type->canImplicitlyConvert(assignment.target->type)) {
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

  if (!ternaryOp.condition->type->canImplicitlyConvert(BoolType::get())) {
    throw runtime_error("Ternary condition must be a boolean");
  } else if (ternaryOp.condition->type != BoolType::get()) {
    ternaryOp.condition =
        make_shared<Cast>(ternaryOp.condition, BoolType::get());
  }

  auto trueType = ternaryOp.thenExpr->type;
  auto falseType = ternaryOp.elseExpr->type;

  if (!trueType->canImplicitlyConvert(falseType) &&
      !falseType->canImplicitlyConvert(trueType)) {
    throw runtime_error("Ternary expressions must have compatible types");
  } else if (trueType != falseType) {
    if (trueType->canImplicitlyConvert(falseType)) {
      ternaryOp.thenExpr = make_shared<Cast>(ternaryOp.thenExpr, falseType);
    } else {
      ternaryOp.elseExpr = make_shared<Cast>(ternaryOp.elseExpr, trueType);
    }
  }

  ternaryOp.type = ternaryOp.thenExpr->type;

  return {};
}