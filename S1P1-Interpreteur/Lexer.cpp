#include "Lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

#include "Debug.h"

void Lexer::Tokenize(const char* path)
{
	ReadFile(path);

	while(current < text.size())
	{
		start = current;
		char c = GetNextChar();

		switch (c)
		{
		case('\n'):
			continue;
		case(' '):
			continue;

		case('.'):
			AddToken(DOT);
			continue;

		case('('):
			AddToken(LPARENTHESIS);
			continue;
		case(')'):
			AddToken(RPARENTHESIS);
			continue;

		case('{'):
			AddToken(LBRACKET);
			continue;
		case('}'):
			AddToken(RBRACKET);
			continue;

		case(','):
			AddToken(COMMA);
			continue;
		case(';'):
			AddToken(SEMICOLON);
			continue;
		case(':'):
			AddToken(COLON);
			continue;
		case('"'):
			AddToken(QUOTE);
			continue;

		case('+'):
			AddToken(ADD);
			continue;
		case('-'):
			AddToken(SUB);
			continue;
		case('*'):
			AddToken(MUL);
			continue;
		case('/'):
			AddToken(DIV);
			continue;

		case('>'):
			AddToken(Match("=", 1) ? SUPEQUAL : SUP);
			continue;
		case('<'):
			AddToken(Match("=", 1) ? INFEQUAL : INF);
			continue;
		case('='):
			AddToken(Match("=", 1) ? EQUAL : ASSIGN);
			continue;
		case('!'):
			AddToken(Match("=", 1) ? NOTEQUAL : NOT);
			continue;

		case('i'):
			if (Match("f", 1))
			{
				AddToken(IF);
				continue;
			}
		case('e'):
			if (Match("lif", 3))
			{
				AddToken(ELIF);
				continue;
			}
			else if (Match("lse", 3))
			{
				AddToken(ELSE);
				continue;
			}

		case('t'):
			if (Match("rue", 3))
			{
				AddToken(TRUE);
				continue;
			}
		case('f'):
			if (Match("alse", 4))
			{
				AddToken(FALSE);
				continue;
			}
			else if (Match("or", 2))
			{
				AddToken(FOR);
				continue;
			}
		case('w'):
			if (Match("hile", 4))
			{
				AddToken(WHILE);
				continue;
			}
		case('r'):
			if (Match("eturn", 5))
			{
				AddToken(RETURN);
				continue;
			}
		case('v'):
			if (Match("al", 2))
			{
				AddToken(VAL);
				continue;
			}
		case('b'):
			if (Match("reak", 4))
			{
				AddToken(BREAK);
				continue;
			}
		case('c'):
			if (Match("ontinue", 7))
			{
				AddToken(CONTINUE);
				continue;
			}

		case('T'):
			if (Match("roop", 4))
			{
				AddToken(TROOP);
				continue;
			}
		}

		if (IsNumber())
		{
			AddToken(NUMBER);
			continue;
		}
		
		if (IsAnIdentifier())
		{
			AddToken(IDENTIFIER);
			continue;
		}

		// ERROR 
		Debug::Log("An error has occured at line : ", line, ", col : ", col, " | First char : ", c, "\n");
		break;
	}

	size_t current_line = 0;
	for (int i = 0; i < token_lexemes.size(); ++i)
	{
		while(token_positions[i].line > current_line)
		{
			current_line++;
			std::cout << std::endl;
		}

		std::cout << RESET << "[" << COLOR << token_lexemes[i] << RESET << "] ";
	}
}

char Lexer::GetNextChar()
{
	char c = text.at(current++);

	if (c == '\n')
	{
		col = 0;
		start = current;
		line++;
	}
	else if (c == ' ')
		start = current;

	col++;
	return c;
}

void Lexer::NextLine()
{

}

void Lexer::ReadFile(const char* path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "File not found !\n";
		abort();
		return;
	}

	text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
}

bool Lexer::IsAnIdentifier()
{
	//Start with the first digit
	current--;
	start = current;

	auto isLetter = [](char c) -> bool
		{
			return (c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c == '_');
		};

	auto isDigit = [](char c) -> bool
		{
			return (c >= 48 && c <= 57);
		};

	size_t i = 0;
	char current_char = text[current];
	bool is_valid = true;

	if (isLetter(current_char) == false) //The first char need to be a letter
		is_valid = false;

	while (current_char != '\0' && current_char != '\n' && current_char != ' ')
	{
		if (isDigit(current_char) == false && isLetter(current_char) == false) //neither a letter or a digit
			break;

		++i;
		current_char = text[current + i];
	}

	if (is_valid)
		current += i;
	else
		current++;

	return is_valid;
}

bool Lexer::IsNumber()
{
	//Start with the first digit
	current--;
	start = current;

	auto isDigit = [](char c) -> bool
		{
			return (c >= 48 && c <= 57);
		};

	size_t i = 0;
	char current_char = text[current];
	int number_type = 1; //0 = invalid | 1 = int | 2 = float

	if (current_char == '.')
		number_type = 0;

	while (current_char != '\0' && current_char != '\n' && current_char != ' ' && current_char != ',')
	{
		if (number_type == 0)
			break;

		if (isDigit(current_char) == false)
		{
			if (number_type == 2) // Multiple .
				number_type = 0;

			if (current_char == '.') // First .
				number_type = 2;
			else
				number_type = 0;
		}

		++i;
		current_char = text[current + i];
	}

	if (text[current - 1 + i] == '.') //if the last char is . or if it's invalid
		number_type = 0;

	if (number_type != 0)
		current += i;
	else
		current++;

	return number_type;
}

void Lexer::AddToken(TokenType type)
{
	AddToken(std::string_view(text.data() + start, current - start), type);
}
void Lexer::AddToken(std::string_view lexeme, TokenType type)
{
	token_lexemes.push_back(lexeme);
	token_types.push_back(type);
	token_positions.push_back(Pos(line, col));
}

bool Lexer::Match(const char* match_text, int size)
{
	size_t i = 0;
	std::string_view text_to_check(text.data() + current, size);

	while (match_text[i] != '\0')
	{
		if (text_to_check[i] != match_text[i])
			return false;

		++i;
	}

	if (current + i < text.size())
	{
		char next_char = text[current + i];
		if (next_char != ' ' && next_char != '\n')
			return false;
	}

	current += size;
	return true;
}
