#include "ast/stmt.hh"

#include "codegen/ir_visitor.hh"
#include "visitor.hh"

Statement::Statement(Tokens tokens) : Node(tokens) {}

Block::Block() : Node(Tokens()), isStandalone(false) {}

Block::Block(Tokens tokens, vector<shared_ptr<Statement>> statements,
             bool isStandalone)
    : Node(tokens), statements(statements), isStandalone(isStandalone) {}

any Block::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

void Block::codegen(IRVisitor &visitor) { visitor.visit(*this); }

ExpressionStatement::ExpressionStatement(Tokens tokens,
                                         shared_ptr<Expr> expression)
    : Statement(tokens), expr(expression) {}

any ExpressionStatement::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

void ExpressionStatement::codegen(IRVisitor &visitor) { visitor.visit(*this); }

VariableDeclaration::VariableDeclaration(Tokens tokens, string name,
                                         shared_ptr<Type> type,
                                         shared_ptr<Expr> initializer)
    : Statement(tokens), name(name), type(type), initializer(initializer) {}

any VariableDeclaration::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

void VariableDeclaration::codegen(IRVisitor &visitor) { visitor.visit(*this); }

Return::Return(Tokens tokens, shared_ptr<Expr> expression)
    : Statement(tokens), expression(expression) {}

any Return::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

void Return::codegen(IRVisitor &visitor) { visitor.visit(*this); }

IfElse::IfElse(Tokens tokens, shared_ptr<Expr> condition, Block thenBlock,
               Block elseBlock)
    : Statement(tokens), condition(condition), thenBlock(thenBlock),
      elseBlock(elseBlock) {}

any IfElse::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

void IfElse::codegen(IRVisitor &visitor) { visitor.visit(*this); }

While::While(Tokens tokens, shared_ptr<Expr> condition, Block body)
    : Statement(tokens), condition(condition), body(body) {}

any While::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }

void While::codegen(IRVisitor &visitor) { visitor.visit(*this); }

StandaloneBlock::StandaloneBlock(Tokens tokens, Block block)
    : Statement(tokens), block(block) {}

any StandaloneBlock::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

void StandaloneBlock::codegen(IRVisitor &visitor) { visitor.visit(*this); }