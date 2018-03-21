project(llvm-kaleidoscope)
set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.9)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(ast kaleidoscope lexer logger parser)

set(SOURCES
        ast/VariableExprAST.cpp
        ast/NumberExprAST.cpp
        ast/CallExprAST.cpp
        ast/FunctionAST.cpp
        ast/BinaryExprAST.cpp
        ast/PrototypeAST.cpp

        kaleidoscope/kaleidoscope.cpp

        lexer/lexer.cpp

        logger/logger.cpp

        parser/parser.cpp)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp ${SOURCES})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(main ${llvm_libs})
