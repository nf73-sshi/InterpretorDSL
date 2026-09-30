#pragma once
#include <string>
#include <vector>

enum TokenType
{
	LPARENTHESIS, RPARENTHESIS,LBRACKET ,RBRACKET,
	SEMICOLON, COLON, COMMA, QUOTE,
	ADD, SUB, MUL, DIV, 

	EQUAL, SUP, INF, SUPEQUAL, INFEQUAL,COMPARE,

	IDENTIFIER,
	NUMBER,

	IF, ELIF, ELSE,
	FOR, WHILE, BREAK, CONTINUE,

	

	TROOP,
};

struct alignas(16) Pos
{
	size_t line = 0, col = 0;
};

struct Lexer
{
	std::string text = {};

	std::vector<TokenType> token_types;
	std::vector<Pos> token_positions;
	std::vector<const char*> token_lexemes;

	void Tokenize(const char* path);

private:
	int start = 0;
	int current = 0;

	int line = 0;
	int col = 0;

	char GetNextChar();

	void ReadFile(const char* path);

	void HandleElement(std::string elt, size_t i);

	void AddToken(char lexeme, TokenType type);
	void AddToken(const char* lexeme, TokenType type);
};

