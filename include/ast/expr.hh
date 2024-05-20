#pragma once

#include "ast.hh"
#include "ast/type.hh"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <memory>

using std::shared_ptr;
using std::string;

struct Expr : public Node {
  shared_ptr<Type> type;

  Expr(Tokens tokens, shared_ptr<Type> type);

  virtual llvm::Value *codegen(IRVisitor &visitor) = 0;

  virtual void setLhs(bool isLhs);
  bool isLhs() const { return isLhs_; }

  virtual bool isLValue() const { return false; }

  virtual shared_ptr<Expr> deepcopy() const = 0;

protected:
  // this can be true only for LValue expressions
  bool isLhs_ = false;
};

struct LValueExpr : public Expr {
  LValueExpr(Tokens tokens, shared_ptr<Type> type);

  bool isLValue() const override { return true; }
};

struct Cast : public Expr {
  shared_ptr<Expr> expr;

  Cast(Tokens tokens, shared_ptr<Expr> expr, shared_ptr<Type> type);
  Cast(shared_ptr<Expr> expr, shared_ptr<Type> type);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct IntLiteral : public Expr {
  long value;

  IntLiteral(Tokens tokens, long value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct FloatLiteral : public Expr {
  double value;

  FloatLiteral(Tokens tokens, double value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct BoolLiteral : public Expr {
  bool value;

  BoolLiteral(Tokens tokens, bool value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct StringLiteral : public Expr {
  string value;

  StringLiteral(Tokens tokens, string value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct ArrayLiteral : public Expr {
  vector<shared_ptr<Expr>> values;

  // todo: not sure about this
  // set after allocation during codegen
  llvm::AllocaInst *alloca;

  ArrayLiteral(Tokens tokens, vector<shared_ptr<Expr>> values);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct StructLiteral : public Expr {
  string name;
  vector<std::pair<string, shared_ptr<Expr>>> fields;

  // todo: not sure about this
  // set after allocation during codegen
  llvm::AllocaInst *alloca;

  StructLiteral(Tokens tokens, string name, vector<string> fieldNames,
                vector<shared_ptr<Expr>> values);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct Pointer : public Expr {
  shared_ptr<Expr> lvalue;

  Pointer(Tokens tokens, shared_ptr<Expr> lvalue);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct VarRef : public LValueExpr {
  string name;

  // set during name resolution for variables
  string mangledName;

  VarRef(Tokens tokens, string name);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct FieldRef : public LValueExpr {
  shared_ptr<Expr> object;
  string field;

  FieldRef(Tokens tokens, shared_ptr<Expr> object, string member);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
  void setLhs(bool isLhs) override;
};

struct Dereference : public LValueExpr {
  shared_ptr<Expr> pointer;

  Dereference(Tokens tokens, shared_ptr<Expr> expr);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
  void setLhs(bool isLhs) override;
};

struct ArrayRef : public LValueExpr {
  shared_ptr<Expr> arrayExpr;
  shared_ptr<Expr> index;

  ArrayRef(Tokens tokens, shared_ptr<Expr> arrayExpr, shared_ptr<Expr> indices);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
  void setLhs(bool isLhs) override;
};

struct FunctionCall : public Expr {
  string callee;
  vector<shared_ptr<Expr>> args;

  // set during name resolution used for call resolution (mangledNames)
  vector<string> callCandidates;

  // set during typechecking once the call was resolved
  string mangledName;

  FunctionCall(Tokens tokens, string callee,
               vector<shared_ptr<Expr>> arguments);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct MethodCall : public FunctionCall {
  shared_ptr<Expr> object;

  MethodCall(Tokens tokens, shared_ptr<Expr> object, string method,
             vector<shared_ptr<Expr>> arguments);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct UnaryPrefixOp : public Expr {
  enum class Operator { Negate, Not };
  Operator op;
  shared_ptr<Expr> operand;

  UnaryPrefixOp(Tokens tokens, Operator op, shared_ptr<Expr> operand);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct BinaryComparison : public Expr {
  enum class Operator { Eq, Ne, Lt, Le, Gt, Ge };
  shared_ptr<Expr> lhs;
  Operator op;
  shared_ptr<Expr> rhs;

  BinaryComparison(Tokens tokens, shared_ptr<Expr> left, Operator op,
                   shared_ptr<Expr> right);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct BinaryArithmetic : public Expr {
  enum class Operator { Add, Sub, Mul, Div, Mod };
  shared_ptr<Expr> lhs;
  Operator op;
  shared_ptr<Expr> rhs;

  BinaryArithmetic(Tokens tokens, shared_ptr<Expr> left, Operator op,
                   shared_ptr<Expr> right);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct BinaryLogical : public Expr {
  enum class Operator { And, Or };
  shared_ptr<Expr> lhs;
  Operator op;
  shared_ptr<Expr> rhs;

  BinaryLogical(Tokens tokens, shared_ptr<Expr> left, Operator op,
                shared_ptr<Expr> right);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct Assignment : public Expr {
  // variable reference or array reference
  shared_ptr<Expr> target;
  shared_ptr<Expr> value;

  Assignment(Tokens tokens, shared_ptr<Expr> target, shared_ptr<Expr> value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct TernaryExpr : public Expr {
  shared_ptr<Expr> condition;
  shared_ptr<Expr> thenExpr;
  shared_ptr<Expr> elseExpr;

  TernaryExpr(Tokens tokens, shared_ptr<Expr> condition,
              shared_ptr<Expr> thenExpr, shared_ptr<Expr> elseExpr);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};

struct VoidExpr : public Expr {
  VoidExpr(Tokens tokens);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
  shared_ptr<Expr> deepcopy() const override;
};