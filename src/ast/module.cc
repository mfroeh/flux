#include "ast/module.hh"
#include "ast/class.hh"

Module::Module(Tokens tokens, std::filesystem::path file,
               vector<std::filesystem::path> includes,
               vector<ClassDefinition> classes,
               vector<FunctionDefinition> functions)
    : Node(std::move(tokens)), includes(std::move(includes)),
      classes(std::move(classes)), functions(std::move(functions)), path(file) {
}

any Module::accept(AbstractAstVisitor &visitor) { return visitor.visit(*this); }