#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Types.h"

struct Environment
{
	std::vector<std::unordered_map<std::string, Object>> datas = {};

	void AddBlock(bool remove = false); // if true, it removes the last block
	bool Exists(size_t block, std::string name) const;
	bool Exists(std::string name) const;
	void Write(std::string name, Object value);
	Object Read(std::string name);
};

