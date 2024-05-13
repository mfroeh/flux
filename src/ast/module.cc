#include "ast/module.hh"

Module::Module(Tokens tokens, vector<unique_ptr<FunctionDefinition>> functions)
    : Node(std::move(tokens)), functions(std::move(functions)) {}

any Module::accept(AstVisitor &visitor) { return visitor.visit(*this); }