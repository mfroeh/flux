#pragma once

#include "ast.hh"
#include "ast/type.hh"
#include <llvm/IR/Value.h>
#include <memory>

using std::string;
using std::unique_ptr;

struct Expr : public Node {
  unique_ptr<Type> type;

  Expr(Tokens tokens, unique_ptr<Type> type);

  virtual llvm::Value *codegen(IRVisitor &visitor) = 0;
};

struct Cast : public Expr {
  unique_ptr<Expr> expr;

  Cast(Tokens tokens, unique_ptr<Expr> expr, unique_ptr<Type> type);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct IntLiteral : public Expr {
  long value;

  IntLiteral(Tokens tokens, long value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct FloatLiteral : public Expr {
  double value;

  FloatLiteral(Tokens tokens, double value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct BoolLiteral : public Expr {
  bool value;

  BoolLiteral(Tokens tokens, bool value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct StringLiteral : public Expr {
  string value;

  StringLiteral(Tokens tokens, string value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct VariableReference : public Expr {
  string name;

  VariableReference(Tokens tokens, string name);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct ArrayReference : public Expr {
  unique_ptr<Expr> array;
  unique_ptr<Expr> index;

  ArrayReference(Tokens tokens, unique_ptr<Expr> array, unique_ptr<Expr> index);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct FunctionCall : public Expr {
  string callee;
  vector<unique_ptr<Expr>> arguments;

  FunctionCall(Tokens tokens, string callee,
               vector<unique_ptr<Expr>> arguments);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct UnaryPrefixOp : public Expr {
  enum class Operator { Negate, Not };
  Operator op;
  unique_ptr<Expr> operand;

  UnaryPrefixOp(Tokens tokens, Operator op, unique_ptr<Expr> operand);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct BinaryComparison : public Expr {
  enum class Operator { Eq, Ne, Lt, Le, Gt, Ge };
  unique_ptr<Expr> left;
  Operator op;
  unique_ptr<Expr> right;

  BinaryComparison(Tokens tokens, unique_ptr<Expr> left, Operator op,
                   unique_ptr<Expr> right);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct BinaryArithmetic : public Expr {
  enum class Operator { Add, Sub, Mul, Div, Mod };
  unique_ptr<Expr> left;
  Operator op;
  unique_ptr<Expr> right;

  BinaryArithmetic(Tokens tokens, unique_ptr<Expr> left, Operator op,
                   unique_ptr<Expr> right);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct BinaryLogical : public Expr {
  enum class Operator { And, Or };
  unique_ptr<Expr> left;
  Operator op;
  unique_ptr<Expr> right;

  BinaryLogical(Tokens tokens, unique_ptr<Expr> left, Operator op,
                unique_ptr<Expr> right);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct Assignment : public Expr {
  string target;
  unique_ptr<Expr> value;

  Assignment(Tokens tokens, string target, unique_ptr<Expr> value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct ArrayAssignment : public Expr {
  string array;
  unique_ptr<Expr> index;
  unique_ptr<Expr> value;

  ArrayAssignment(Tokens tokens, string array, unique_ptr<Expr> index,
                  unique_ptr<Expr> value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct CompoundAssignment : public Expr {
  enum class Operator { Add, Sub, Mul, Div, Mod };
  string target;
  Operator op;
  unique_ptr<Expr> value;

  CompoundAssignment(Tokens tokens, string target, Operator op,
                     unique_ptr<Expr> value);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct TernaryExpr : public Expr {
  unique_ptr<Expr> condition;
  unique_ptr<Expr> thenExpr;
  unique_ptr<Expr> elseExpr;

  TernaryExpr(Tokens tokens, unique_ptr<Expr> condition,
              unique_ptr<Expr> thenExpr, unique_ptr<Expr> elseExpr);

  virtual any accept(class AstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};
