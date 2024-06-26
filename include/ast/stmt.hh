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
  virtual void codegen(struct IRVisitor &visitor) = 0;
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
  void codegen(struct IRVisitor &visitor);
};

struct ExpressionStatement : public Statement {
  shared_ptr<Expr> expr;

  ExpressionStatement(Tokens tokens, shared_ptr<Expr> expression);
  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct VariableDeclaration : public Statement {
  string name;
  shared_ptr<Type> type;
  shared_ptr<Expr> initializer;

  // set during name resolution
  string mangledName;

  VariableDeclaration(Tokens tokens, string name, shared_ptr<Type> type,
                      shared_ptr<Expr> initializer);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct Return : public Statement {
  shared_ptr<Expr> expression;

  Return(Tokens tokens, shared_ptr<Expr> expression);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct IfElse : public Statement {
  shared_ptr<Expr> condition;
  Block thenBlock;
  Block elseBlock;

  IfElse(Tokens tokens, shared_ptr<Expr> condition, Block thenBlock,
         Block elseBlock);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct While : public Statement {
  shared_ptr<Expr> condition;
  Block body;

  While(Tokens tokens, shared_ptr<Expr> condition, Block body);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct StandaloneBlock : public Statement {
  Block block;

  StandaloneBlock(Tokens tokens, Block block);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};

struct Print : public Statement {
  string format;
  vector<shared_ptr<Expr>> args;

  Print(Tokens tokens, string format, vector<shared_ptr<Expr>> expr);

  any accept(AbstractAstVisitor &visitor) override;
  void codegen(struct IRVisitor &visitor) override;
};