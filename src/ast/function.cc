#include "ast/function.hh"
#include "ir_visitor.hh"
#include "visitor.hh"
#include <llvm/IR/Value.h>

Parameter::Parameter(Tokens tokens, string name, unique_ptr<Type> type)
    : Node(std::move(tokens)), name(std::move(name)), type(std::move(type)) {}

any Parameter::accept(AstVisitor &visitor) { return visitor.visit(*this); }

FunctionDefinition::FunctionDefinition(Tokens tokens, string name,
                                       vector<Parameter> parameters,
                                       unique_ptr<Type> returnType,
                                       unique_ptr<Block> body)
    : Node(std::move(tokens)), name(std::move(name)),
      parameters(std::move(parameters)), returnType(std::move(returnType)),
      body(std::move(body)) {}

any FunctionDefinition::accept(AstVisitor &visitor) {
  return visitor.visit(*this);
}

llvm::Function *FunctionDefinition::codegen(IRVisitor &visitor) {
  return visitor.visit(*this);
}