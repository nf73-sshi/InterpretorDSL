#include "Lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>

void Lexer::Tokenize(const char* path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "File not found !\n";
		abort();
		return;
	}

	std::string line;
	std::string elt; //element

	if (line.find("if") != std::string::npos)
		std::cout << "if found !\n";

	size_t i = 0; //elt count
	size_t l = 1;
	size_t c = 1;

	while (getline(file, line))
	{
		while (!line.empty())
		{
			size_t elt_pos = line.find(" ");

			if (elt_pos == std::string::npos)
			{
				elt = line.substr(0, line.size());
				line = {};
			}
			else
			{
				elt = line.substr(0, elt_pos);
				line = line.substr(elt_pos + 1, line.size() - elt_pos - 1);
			}

			std::cout << "[" << elt << "]"; //TODO treat the element

			HandleElement(elt, i);
			token_names.push_back(elt);
			token_positions.push_back({l, c});
		}

		std::cout << "\n";
	}
}

void Lexer::HandleElement(std::string elt, size_t i)
{
	auto isDigit = [](char c) -> bool
	{
		return (c >= 48 && c <= 57);
	};

	if (isDigit(elt[0])) //NUMBER
	{
		int number_type = 1; //0 = invalid | 1 = int | 2 = float

		if (elt.back() == '.')
			number_type = 0;
		else
		{
			for (int i = 0; i < elt.size(); ++i)
			{
				if (isDigit(elt[i]) == false)
				{
					if (number_type == 2) // Multiple .
					{
						number_type = 0;
						break;
					}

					if (elt[i] == '.') // First .
						number_type = 2;
					else
					{
						number_type = 0;
						break;
					}
				}

				continue;
			}
		}
	}
}
