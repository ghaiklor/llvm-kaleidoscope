SOURCES = $(shell find ast kaleidoscope lexer logger parser -name '*.cpp')
HEADERS = $(shell find ast kaleidoscope lexer logger parser -name '*.h')
OBJ = ${SOURCES:.cpp=.o}

CC = llvm-g++
# -stdlib=libc++ -std=c++11
CFLAGS = -g -O3 -I llvm/include -I llvm/build/include -I ./
LLVMFLAGS = `/usr/local/Cellar/llvm/3.9.1_1/bin/llvm-config --cxxflags --ldflags --system-libs --libs all`

.PHONY: main

main: main.cpp ${OBJ}
	${CC} ${CFLAGS} ${LLVMFLAGS} ${OBJ} $< -o $@

clean:
	rm -r ${OBJ}

%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} ${LLVMFLAGS} -c $< -o $@
