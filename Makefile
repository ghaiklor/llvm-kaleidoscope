.PHONY: compile

compile:
	llvm-g++ -g -O3 -stdlib=libc++ -std=c++11 -I llvm/include -I llvm/build/include lexer.cpp
