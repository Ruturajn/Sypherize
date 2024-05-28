# Set the minimum required version for CMake.
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# Add flags that need to be passed to the compiler
SET(GXX_FLAGS "-ggdb -Wall -Werror -Wextra -pedantic")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")

set(CMAKE_VERBOSE_MAKEFILE ON)

# Set the name of the project.
PROJECT(sypherc)

# Set the source files.
SET(SYPHER_SRCS
    src/ast_decls.cpp
    src/ast_exprs.cpp
    src/ast_prog.cpp
    src/ast_stmts.cpp
    src/ast_types.cpp
    src/cmdops.cpp
    src/diagnostics.cpp
    src/lexer.cpp
    src/main.cpp
    src/parser.cpp)

# Set the output directory for the binary, and create it if it doesn't exist.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
FILE(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Specify the source files.
ADD_EXECUTABLE(sypherc "${SYPHER_SRCS}")

# Set Cmake standard.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for the build.
target_include_directories(sypherc PUBLIC
                           "${PROJECT_SOURCE_DIR}/inc/"
                           )
