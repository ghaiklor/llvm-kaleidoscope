#ifndef __LEXER_H__
#define __LEXER_H__

#include <cstdlib>
#include <string>

// Provide a simple token buffer
// CurTok is the current token the parser is looking at
// getNextToken reads another token from the lexer and updates CurTok with its results
int CurTok;
int gettok();
int getNextToken();

// If the current token is an identifier
// IdentifierStr will hold the name of the identifier
std::string IdentifierStr;

// If the current token is a numeric literal
// NumVal holds its value
double NumVal;

#endif
