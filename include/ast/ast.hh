#pragma once

#include "ParserRuleContext.h"
#include <any>
#include <memory>
#include <string>
#include <vector>

using std::any;
using std::ostream;
using std::shared_ptr;
using std::string;
using std::vector;

struct Position {
  int line;
  int column;

  Position(int line, int column);
};

struct Token {
  Position position;
  string text;

  Token(int line, int column, string text);
};

struct Tokens {
  vector<Token> tokens;

  Tokens() = default;
  Tokens(antlr4::ParserRuleContext *ctx);
  Tokens(vector<Token> tokens);

  Position getStart() const;
  Position getEnd() const;
};

struct Node {
  Tokens tokens;

  Node(Tokens tokens);
  virtual ~Node() = default;

  virtual any accept(class AbstractAstVisitor &visitor) = 0;

  virtual ostream &print(ostream &out) const;
};

ostream &operator<<(ostream &out, const Node &node);