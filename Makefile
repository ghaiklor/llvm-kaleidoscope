.PHONY: compile

compile:
	llvm-g++ -g -O3 -stdlib=libc++ -std=c++11 `/usr/local/Cellar/llvm/3.9.1_1/bin/llvm-config --cxxflags --ldflags --system-libs --libs core` -I llvm/include -I llvm/build/include lexer.cpp -o lexer.out
