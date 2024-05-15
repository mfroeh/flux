#pragma once

#include "symbol_table.hh"
#include <filesystem>

struct ModuleContext {
  ModuleContext(std::filesystem::path path) : path(std::move(path)) {}

  SymbolTable symbolTable;
  std::filesystem::path path;
};