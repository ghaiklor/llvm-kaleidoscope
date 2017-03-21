SOURCES = $(shell find ast lexer logger parser -name '*.cpp')
HEADERS = $(shell find ast lexer logger parser -name '*.h')
OBJ = ${SOURCES:.cpp=.o}
CC = llvm-g++

CFLAGS = -g -O3 -stdlib=libc++ -std=c++11 -I llvm/include -I llvm/build/include
# LLVMFLAGS = `/usr/local/Cellar/llvm/3.9.1_1/bin/llvm-config --cxxflags --ldflags`

.PHONY: compile

compile: ${OBJ}

%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@
