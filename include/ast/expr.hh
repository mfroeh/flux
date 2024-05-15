#pragma once

#include "ast.hh"
#include "ast/type.hh"
#include <llvm/IR/Value.h>
#include <memory>

using std::shared_ptr;
using std::string;

struct Expr : public Node {
  shared_ptr<Type> type;

  Expr(Tokens tokens, shared_ptr<Type> type);

  virtual llvm::Value *codegen(IRVisitor &visitor) = 0;
};

struct Cast : public Expr {
  shared_ptr<Expr> expr;

  Cast(Tokens tokens, shared_ptr<Expr> expr, shared_ptr<Type> type);
  Cast(shared_ptr<Expr> expr, shared_ptr<Type> type);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct IntLiteral : public Expr {
  long value;

  IntLiteral(Tokens tokens, long value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct FloatLiteral : public Expr {
  double value;

  FloatLiteral(Tokens tokens, double value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct BoolLiteral : public Expr {
  bool value;

  BoolLiteral(Tokens tokens, bool value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct StringLiteral : public Expr {
  string value;

  StringLiteral(Tokens tokens, string value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct VariableReference : public Expr {
  string name;

  // set during name resolution
  string mangledName;

  VariableReference(Tokens tokens, string name);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct ArrayReference : public Expr {
  shared_ptr<Expr> arrayExpr;
  shared_ptr<Expr> index;

  ArrayReference(Tokens tokens, shared_ptr<Expr> arrayExpr,
                 shared_ptr<Expr> indices);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct FunctionCall : public Expr {
  string callee;
  vector<shared_ptr<Expr>> arguments;

  // set during variable resolution, used for call resolution (mangledNames)
  vector<string> callCandidates;

  // set during TODO
  string mangledName;

  FunctionCall(Tokens tokens, string callee,
               vector<shared_ptr<Expr>> arguments);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct UnaryPrefixOp : public Expr {
  enum class Operator { Negate, Not };
  Operator op;
  shared_ptr<Expr> operand;

  UnaryPrefixOp(Tokens tokens, Operator op, shared_ptr<Expr> operand);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
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
};

struct Assignment : public Expr {
  // variable reference or array reference
  shared_ptr<Expr> target;
  shared_ptr<Expr> value;

  Assignment(Tokens tokens, shared_ptr<Expr> target, shared_ptr<Expr> value);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct TernaryExpr : public Expr {
  shared_ptr<Expr> condition;
  shared_ptr<Expr> thenExpr;
  shared_ptr<Expr> elseExpr;

  TernaryExpr(Tokens tokens, shared_ptr<Expr> condition,
              shared_ptr<Expr> thenExpr, shared_ptr<Expr> elseExpr);

  virtual any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};
