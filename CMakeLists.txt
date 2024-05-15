cmake_minimum_required(VERSION 3.17)

project(
  flux
  VERSION 1.0.0
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

find_package(antlr4-runtime REQUIRED)
find_package(antlr4-generator REQUIRED)
# set by nix environment
set(ANTLR4_JAR_LOCATION $ENV{ANTLR4_JAR_LOCATION})

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

find_package(argparse REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(magic_enum REQUIRED)

# interpreter
find_package(FFI REQUIRED) # required for interpreter
set(INTERPRETER_LIBS executionengine interpreter mcjit)

# https://github.com/antlr/antlr4/blob/master/runtime/Cpp/cmake/Antlr4Package.md
antlr4_generate(FluxLexer ${PROJECT_SOURCE_DIR}/grammar/FluxLexer.g4 LEXER
                FALSE FALSE)
antlr4_generate(
  FluxParser
  ${PROJECT_SOURCE_DIR}/grammar/FluxParser.g4
  PARSER
  FALSE
  TRUE
  ""
  ${ANTLR4_TOKEN_FILES_FluxLexer}
  ${ANTLR4_TOKEN_DIRECTORY_FluxLexer})

set(parser "")

set(ast
    src/ast/ast_creator.cc
    src/ast/ast.cc
    src/ast/expr.cc
    src/ast/function.cc
    src/ast/module.cc
    src/ast/stmt.cc
    src/ast/sugar.cc
    src/ast/type.cc
    )

set(analysis src/analysis/desugar.cc src/analysis/scope.cc src/analysis/variable_resolver.cc src/analysis/typecheck.cc)

set(codegen "")

set(toplevel "")

add_executable(
  flux
  src/main.cc
  src/visitor.cc
  src/symbol_table.cc
  ${toplevel}
  ${parser}
  ${ast}
  ${analysis}
  ${codegen}
  ${ANTLR4_SRC_FILES_FluxLexer}
  ${ANTLR4_SRC_FILES_FluxParser})

target_include_directories(
  flux
  PRIVATE include/ ${magic_enum_include_dir} ${ANTLR4_INCLUDE_DIR}
          ${ANTLR4_INCLUDE_DIR_FluxLexer} ${ANTLR4_INCLUDE_DIR_FluxParser}
          ${LLVM_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(
  llvm_libs
  support
  core
  irreader
  passes
  instrumentation
  ${INTERPRETER_LIBS}
  ${LLVM_TARGETS_TO_BUILD})

target_link_libraries(
  flux PRIVATE argparse::argparse Boost::system magic_enum::magic_enum FFI::ffi
               antlr4-runtime ${llvm_libs})

install(TARGETS flux DESTINATION bin)
