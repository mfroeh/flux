#pragma once

#include "scope.hh"
#include <any>
#include <memory>
#include <string>
#include <vector>

using std::any;
using std::ostream;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
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

  Tokens(vector<Token> tokens);

  Position getStart() const;
  Position getEnd() const;
};

struct Node {
  Tokens tokens;
  shared_ptr<Scope> scope;

  Node(Tokens tokens);
  virtual ~Node() = default;

  void setScope(shared_ptr<Scope> scope);

  virtual any accept(class AstVisitor &visitor) = 0;

  virtual ostream &print(ostream &out) const = 0;
};

ostream &operator<<(ostream &out, const Node &node);