#pragma once

#include "ast/expr.hh"
#include "ast/function.hh"
#include "ast/stmt.hh"
#include "scope.hh"
#include "symbol_table.hh"
#include "visitor.hh"

using std::shared_ptr;

class VariableResolver : public AstVisitor {
public:
  VariableResolver(ModuleContext &context, SymbolTable &symTab);

  // opens new scope
  any visit(struct FunctionDefinition &functionDefinition) override;
  any visit(struct IfElse &ifElse) override;
  any visit(struct While &whileLoop) override;
  any visit(struct StandaloneBlock &forLoop) override;

  // creates variable in current scope
  any visit(struct Parameter &parameter) override;
  any visit(struct VariableDeclaration &variableDeclaration) override;

  // resolves variable references
  any visit(struct VariableReference &var) override;
  any visit(struct ArrayReference &arr) override;

  // finds function call candidates
  any visit(struct FunctionCall &funcCall) override;

private:
  shared_ptr<Scope> currentScope;
};