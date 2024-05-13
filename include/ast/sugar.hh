#pragma once

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"

namespace sugar {

struct ElifStatement : public Statement {
  unique_ptr<Expr> condition;
  unique_ptr<Block> thenBlock;

  ElifStatement(Tokens tokens, unique_ptr<Expr> condition,
                unique_ptr<Block> thenBlock);
};

struct IfElifElseStatement : public Statement {
  unique_ptr<Expr> condition;
  unique_ptr<Block> thenBlock;
  vector<unique_ptr<ElifStatement>> elseIfs;
  unique_ptr<Block> elseBlock;

  IfElifElseStatement(Tokens tokens, unique_ptr<Expr> condition,
                      unique_ptr<Block> thenBlock,
                      vector<unique_ptr<ElifStatement>> elifs,
                      unique_ptr<Block> elseBlock);
};

struct ForStatement : public Statement {
  unique_ptr<Statement> initializer;
  unique_ptr<Expr> condition;
  unique_ptr<Statement> update;
  unique_ptr<Block> body;

  ForStatement(Tokens tokens, unique_ptr<Statement> initializer,
               unique_ptr<Expr> condition, unique_ptr<Statement> update,
               unique_ptr<Block> body);
};

struct InIntervalExpr : public Expr {
  unique_ptr<Expr> value;
  unique_ptr<Expr> lower;
  unique_ptr<Expr> upper;

  InIntervalExpr(Tokens tokens, unique_ptr<Expr> value, unique_ptr<Expr> lower,
                 unique_ptr<Expr> upper);
};

} // namespace sugar