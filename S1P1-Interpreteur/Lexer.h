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
	FOR, WHILE, BREAK, CONTINUE, RETURN,
	TRUE, FALSE,

	IDENTIFIER,
	NUMBER,
	TROOP,
};

struct alignas(16) Pos
{
	size_t line = 0, col = 0;
};

class function
{
	public:
		virtual function() = default;
};
class Statement
{
public:
	 virtual Statement() = default;
};
class VariableStatement : public Statement
{
	std::string name;
	std::vector<function> functionsList;
};
struct Parser
{
	int actual = 0;
	std::vector<TokenType> token_list;
	TokenType peek(){ return token_list[actual];}
	TokenType previous() { return token_list[actual - 1]; }
	void ReadToken();
};

struct Token
{
	TokenType type;
	std::string_view lexeme;
	Pos pos;

};

struct Lexer
{
	std::string text = {};

	std::vector<Token> tokens;

	void Tokenize(const char* path);

private:
	int start = 0;
	int current = 0;

	int line = 0;
	int col = 0;

	char GetNextChar();

	void ReadFile(const char* path);

	bool IsAnIdentifier();
	bool IsNumber();

	void AddToken(TokenType type);
	void AddToken(std::string_view lexeme, TokenType type);

	bool Match(const char* match_text, int size); //if remove_last it wont add the last character of the string (like space !)
};

