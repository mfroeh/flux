#pragma once

#include "ast/ast.hh"
#include "ast/expr.hh"
#include "ast/type.hh"
#include <any>
#include <memory>

using std::shared_ptr;
using std::string;
using std::vector;

struct Statement : public Node {
  Statement(Tokens tokens);
};

// Eventually make this std::optional in all of the types that have it
struct Block : public Node {
  vector<shared_ptr<Statement>> statements;
  bool isStandalone;

  Block();

  Block(Tokens tokens, vector<shared_ptr<Statement>> statements,
        bool isStandalone);

  size_t size() const { return statements.size(); }

  bool isEmpty() const { return statements.empty(); }

  any accept(AbstractAstVisitor &visitor) override;
};

struct ExpressionStatement : public Statement {
  shared_ptr<Expr> expr;

  ExpressionStatement(Tokens tokens, shared_ptr<Expr> expression);

  any accept(AbstractAstVisitor &visitor) override;
};

struct VariableDeclaration : public Statement {
  string name;
  shared_ptr<Type> type;
  shared_ptr<Expr> initializer;

  VariableDeclaration(Tokens tokens, string name, shared_ptr<Type> type,
                      shared_ptr<Expr> initializer);

  any accept(AbstractAstVisitor &visitor) override;
};

struct Return : public Statement {
  shared_ptr<Expr> expression;

  Return(Tokens tokens, shared_ptr<Expr> expression);

  any accept(AbstractAstVisitor &visitor) override;
};

struct IfElse : public Statement {
  shared_ptr<Expr> condition;
  Block thenBlock;
  Block elseBlock;

  IfElse(Tokens tokens, shared_ptr<Expr> condition, Block thenBlock,
         Block elseBlock);

  any accept(AbstractAstVisitor &visitor) override;
};

struct While : public Statement {
  shared_ptr<Expr> condition;
  Block body;

  While(Tokens tokens, shared_ptr<Expr> condition, Block body);

  any accept(AbstractAstVisitor &visitor) override;
};

struct StandaloneBlock : public Statement {
  Block block;

  StandaloneBlock(Tokens tokens, Block block);

  any accept(AbstractAstVisitor &visitor) override;
};