#pragma once
#include <string>
#include <vector>

enum TokenType
{
	LPARENTHESIS, RPARENTHESIS,
	SEMICOLON, COLON, COMMA, QUOTE,

	IDENTIFIER,
	NUMBER,

	IF, ELIF, ELSE,
	FOR, WHILE, BREAK, CONTINUE,

	TROOP,
};

struct alignas(8) Pos
{
	int line = 0, col = 0;
};

struct Lexer
{
	std::vector<TokenType> token_types;
	std::vector<Pos> token_positions;
	std::vector<std::string> token_names;

	void Tokenize(const char* path);

private:
	void HandleElement(const char* element);
};

