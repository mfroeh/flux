#include "ast/function.hh"
#include "codegen/ir_visitor.hh"
#include "visitor.hh"
#include <llvm/IR/Value.h>

using namespace std;

Parameter::Parameter(Tokens tokens, string name, shared_ptr<Type> type)
    : Node(std::move(tokens)), name(std::move(name)), type(std::move(type)) {}

any Parameter::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

FunctionDefinition::FunctionDefinition(Tokens tokens, string name,
                                       vector<Parameter> parameters,
                                       shared_ptr<Type> returnType, Block body)
    : Statement(std::move(tokens)), name(std::move(name)),
      parameters(std::move(parameters)), returnType(std::move(returnType)),
      body(std::move(body)) {}

any FunctionDefinition::accept(AbstractAstVisitor &visitor) {
  return visitor.visit(*this);
}

void FunctionDefinition::codegen(IRVisitor &visitor) { visitor.visit(*this); }