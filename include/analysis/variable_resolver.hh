#pragma once

#include "ast/class.hh"
#include "scope.hh"
#include "symbol_table.hh"
#include "visitor.hh"

using std::shared_ptr;

class VariableResolver : public AstVisitor {
public:
  VariableResolver(ModuleContext &context, SymbolTable &symTab);

  // opens new scope
  any visit(struct ClassDefinition &classDefinition) override;
  any visit(struct FunctionDefinition &functionDefinition) override;
  any visit(struct IfElse &ifElse) override;
  any visit(struct While &whileLoop) override;
  any visit(struct StandaloneBlock &forLoop) override;

  // creates variable in current scope
  any visit(struct Parameter &parameter) override;
  any visit(struct VariableDeclaration &variableDeclaration) override;
  any visit(struct FieldDeclaration &fieldDeclaration) override;

  // resolves variable references
  any visit(struct VarRef &var) override;
  any visit(struct ArrayRef &arr) override;

  // finds function call candidates
  any visit(struct FunctionCall &funcCall) override;

private:
  shared_ptr<Scope> currentScope;
};