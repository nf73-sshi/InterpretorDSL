#pragma once
#include <string>
#include <vector>

enum TokenType
{
	LPARENTHESIS, RPARENTHESIS, LBRACKET, RBRACKET,
	SEMICOLON, COLON, COMMA, QUOTE, VAL,
	ADD, SUB, MUL, DIV, DOT,

	ASSIGN, SUP, INF, SUPEQUAL, INFEQUAL, EQUAL,
	NOT, NOTEQUAL,

	IF, ELIF, ELSE,
	FOR, WHILE, BREAK, CONTINUE,
	TRUE, FALSE,

	IDENTIFIER,
	NUMBER,
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
	std::vector<std::string_view> token_lexemes;

	void Tokenize(const char* path);

private:
	int start = 0;
	int current = 0;

	int line = 0;
	int col = 0;

	char GetNextChar();
	void NextLine();

	void ReadFile(const char* path);

	bool IsAnIdentifier();
	bool IsNumber();

	void AddToken(TokenType type);
	void AddToken(std::string_view lexeme, TokenType type);

	bool Match(const char* match_text, int size); //if remove_last it wont add the last character of the string (like space !)
};

