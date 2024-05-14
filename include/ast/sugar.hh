#pragma once

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"

namespace sugar {

struct ElifStatement : public Statement {
  shared_ptr<Expr> condition;
  Block thenBlock;

  ElifStatement(Tokens tokens, shared_ptr<Expr> condition, Block thenBlock);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct IfElifElseStatement : public Statement {
  shared_ptr<Expr> condition;
  Block thenBlock;
  vector<shared_ptr<ElifStatement>> elseIfs;
  Block elseBlock;

  IfElifElseStatement(Tokens tokens, shared_ptr<Expr> condition,
                      Block thenBlock, vector<shared_ptr<ElifStatement>> elifs,
                      Block elseBlock);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct ForLoop : public Statement {
  shared_ptr<Statement> initializer;
  shared_ptr<Expr> condition;
  shared_ptr<Statement> update;
  Block body;

  ForLoop(Tokens tokens, shared_ptr<Statement> initializer,
          shared_ptr<Expr> condition, shared_ptr<Statement> update, Block body);

  any accept(class AbstractAstVisitor &visitor) override;
};

struct InIntervalExpr : public Expr {
  enum IntervalKind { Open, Closed, OpenClosed, ClosedOpen } kind;
  shared_ptr<Expr> value;
  shared_ptr<Expr> lower;
  shared_ptr<Expr> upper;

  InIntervalExpr(Tokens tokens, shared_ptr<Expr> value, shared_ptr<Expr> lower,
                 shared_ptr<Expr> upper, IntervalKind intervalKind);

  any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

struct CompoundAssignment : public Expr {
  // variable reference or array reference
  shared_ptr<Expr> target;
  BinaryArithmetic::Operator op;
  shared_ptr<Expr> value;

  CompoundAssignment(Tokens tokens, shared_ptr<Expr> target,
                     BinaryArithmetic::Operator op, shared_ptr<Expr> value);

  any accept(class AbstractAstVisitor &visitor) override;
  llvm::Value *codegen(IRVisitor &visitor) override;
};

} // namespace sugar