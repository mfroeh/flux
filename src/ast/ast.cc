#include "ast/ast.hh"
#include <cassert>

Position::Position(int line, int column) : line(line), column(column) {}

Token::Token(int line, int column, string text)
    : position(line, column), text(std::move(text)) {}

Tokens::Tokens(vector<Token> tokens) : tokens(std::move(tokens)) {}

Position Tokens::getStart() const { return tokens.front().position; }

Position Tokens::getEnd() const { return tokens.back().position; }

Node::Node(Tokens tokens) : tokens(std::move(tokens)) {}

void Node::setScope(shared_ptr<Scope> scope) {
  assert(scope == nullptr);
  this->scope = scope;
}

ostream &operator<<(ostream &out, const Node &node) { return node.print(out); }