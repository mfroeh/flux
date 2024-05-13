#include "ast/sugar.hh"
#include "ast/type.hh"

using namespace sugar;
using namespace std;

ElifStatement::ElifStatement(Tokens tokens, unique_ptr<Expr> condition,
                             unique_ptr<Block> thenBlock)
    : Statement(std::move(tokens)), condition(std::move(condition)),
      thenBlock(std::move(thenBlock)) {}

IfElifElseStatement::IfElifElseStatement(
    Tokens tokens, unique_ptr<Expr> condition, unique_ptr<Block> thenBlock,
    vector<unique_ptr<ElifStatement>> elifs, unique_ptr<Block> elseBlock)
    : Statement(std::move(tokens)), condition(std::move(condition)),
      thenBlock(std::move(thenBlock)), elseIfs(std::move(elifs)),
      elseBlock(std::move(elseBlock)) {}

ForStatement::ForStatement(Tokens tokens, unique_ptr<Statement> initializer,
                           unique_ptr<Expr> condition,
                           unique_ptr<Statement> update, unique_ptr<Block> body)
    : Statement(std::move(tokens)), initializer(std::move(initializer)),
      condition(std::move(condition)), update(std::move(update)),
      body(std::move(body)) {}

InIntervalExpr::InIntervalExpr(Tokens tokens, unique_ptr<Expr> value,
                               unique_ptr<Expr> lower, unique_ptr<Expr> upper)
    : Expr(std::move(tokens), make_unique<InferType>()),
      value(std::move(value)), lower(std::move(lower)),
      upper(std::move(upper)) {}
