#pragma once

#include <filesystem>

struct ModuleContext {
  ModuleContext(std::filesystem::path path) : path(std::move(path)) {}

  std::filesystem::path path;
};