#include "ast/ast.hh"
#include "antlr4-runtime/Token.h"
#include "antlr4-runtime/tree/TerminalNode.h"
#include <cassert>
#include <ostream>

Position::Position(int line, int column) : line(line), column(column) {}

Token::Token(int line, int column, string text)
    : position(line, column), text(std::move(text)) {}

Tokens::Tokens(antlr4::ParserRuleContext *ctx) {
  for (auto &term : ctx->children) {
    if (auto terminal = dynamic_cast<antlr4::tree::TerminalNode *>(term)) {
      auto symbol = terminal->getSymbol();
      tokens.emplace_back(symbol->getLine(),
                          symbol->getCharPositionInLine() + 1,
                          symbol->getText());
    }
  }
}

Tokens::Tokens(vector<Token> tokens) : tokens(std::move(tokens)) {}

Position Tokens::getStart() const { return tokens.front().position; }

Position Tokens::getEnd() const { return tokens.back().position; }

Node::Node(Tokens tokens) : tokens(std::move(tokens)) {}

ostream &Node::print(ostream &out) const {
  for (auto &token : tokens.tokens) {
    out << token.text << " ";
  }
  return out;
}

ostream &operator<<(ostream &out, const Node &node) { return node.print(out); }