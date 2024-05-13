#pragma once

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/type.hh"
#include <memory>

using std::string;
using std::unique_ptr;
using std::vector;

struct Statement : public Node {
  Statement(Tokens tokens);
};

struct Block : public Node {
  vector<unique_ptr<Statement>> statements;

  Block(Tokens tokens, vector<unique_ptr<Statement>> statements);
};

struct ExpressionStatement : public Statement {
  unique_ptr<Expr> expression;

  ExpressionStatement(Tokens tokens, unique_ptr<Expr> expression);
};

struct VariableDeclaration : public Statement {
  string name;
  unique_ptr<Type> type;
  unique_ptr<Expr> initializer;

  VariableDeclaration(Tokens tokens, string name, unique_ptr<Type> type,
                      unique_ptr<Expr> initializer);
};

struct Return : public Statement {
  unique_ptr<Expr> expression;

  Return(Tokens tokens, unique_ptr<Expr> expression);
};

struct IfElse : public Statement {
  unique_ptr<Expr> condition;
  unique_ptr<Block> thenBlock;
  unique_ptr<Block> elseBlock;

  IfElse(Tokens tokens, unique_ptr<Expr> condition, unique_ptr<Block> thenBlock,
         unique_ptr<Block> elseBlock);
};

struct WhileStatement : public Statement {
  unique_ptr<Expr> condition;
  unique_ptr<Block> body;

  WhileStatement(Tokens tokens, unique_ptr<Expr> condition,
                 unique_ptr<Block> body);
};