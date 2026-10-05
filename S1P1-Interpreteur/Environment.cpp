#include "Environment.h"
#include "Parser.h"
#include "Debug.h"

void Environment::AddBlock(bool remove)
{
	if (remove == false)
		datas.push_back(std::unordered_map<std::string, Object>());
	else
		datas.pop_back();
}

bool Environment::Exists(size_t block, std::string name) const
{
	if (block >= datas.size())
		return false; //out of scope

	return datas[block].contains(name);
}

void Environment::Write(std::string name, Object value)
{
	size_t current_block = datas.size() - 1;

	while (current_block >= 0)
	{
		if (Exists(current_block, name))
		{
			datas[current_block][name] = value;
			break;
		}

		current_block--;
	}
}

Object Environment::Read(std::string name)
{
	size_t current_block = datas.size() - 1;

	while (current_block >= 0)
	{
		if (Exists(current_block, name))
		{
			return datas[current_block][name];
		}

		current_block--;
	}

	Debug::FatalError("UNDEFINED VARIABLE", name);
	return Object{ std::monostate{} };
}
