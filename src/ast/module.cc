#include "ast/module.hh"

Module::Module(Tokens tokens, vector<FunctionDefinition> functions)
    : Node(std::move(tokens)), functions(std::move(functions)) {}

any Module::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }