#include "symbol.hh"
#include "ast/function.hh"
#include "ast/type.hh"

using namespace std;

ostream &operator<<(std::ostream &os, const VariableSymbol &symbol) {
  os << "VariableSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
     << *symbol.type << ")";
  return os;
}

ostream &operator<<(std::ostream &os, const FunctionSymbol &symbol) {
  os << "FunctionSymbol(" << symbol.name << ", " << symbol.mangledName << ", "
     << *symbol.returnType << ", [";
  for (auto &param : symbol.parameters) {
    os << param->name << ": " << *param->type << ", ";
  }
  os << "], "
     << "-> " << *symbol.returnType << ")";
  return os;
}

ostream &operator<<(std::ostream &os, const ClassSymbol &symbol) {
  os << "ClassSymbol(" << symbol.name << ", " << *symbol.type << ")";
  return os;
}

VariableSymbol::VariableSymbol(string name, string mangledName,
                               shared_ptr<Type> type, int depth, int count)
    : Symbol(depth, count), name(std::move(name)),
      mangledName(std::move(mangledName)), type(std::move(type)) {}

FunctionSymbol::FunctionSymbol(string name, string mangledName,
                               shared_ptr<Type> returnType,
                               const vector<Parameter> &parameters, int depth,
                               int count)
    : Symbol(depth, count), name(std::move(name)),
      mangledName(std::move(mangledName)), returnType(std::move(returnType)) {
  for (auto &param : parameters) {
    this->parameters.push_back(make_shared<Parameter>(param));
  }
}

ClassSymbol::ClassSymbol(string name, shared_ptr<Type> type)
    : Symbol(0, 0), name(name), type(std::move(type)) {}