#include "ast/module.hh"
#include "ast/class.hh"

Module::Module(Tokens tokens, vector<ClassDefinition> classes,
               vector<FunctionDefinition> functions)
    : Node(std::move(tokens)), classes(std::move(classes)),
      functions(std::move(functions)) {}

any Module::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }