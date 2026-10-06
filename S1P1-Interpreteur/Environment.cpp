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

bool Environment::Exists(std::string name) const
{
	int current_block = datas.size() - 1;

	while (current_block >= 0)
	{
		if (Exists(current_block, name))
		{
			return true;
		}

		current_block--;
	}

	return false;
}

void Environment::Write(std::string name, Object value)
{
	Debug::Log("Var written : ", name);

	if (std::holds_alternative<float>(value))
	{
		std::cout << " = " << std::get<float>(value) << "\n";
	}
	else if (std::holds_alternative<Troop>(value))
	{
		std::cout << " = Troop\n";
	}

	size_t current_block = datas.size() - 1;
	datas[current_block][name] = value;
}

Object Environment::Read(std::string name)
{
	int current_block = datas.size() - 1;

	while (current_block >= 0)
	{
		if (Exists(current_block, name))
		{
			Debug::Log("Var read : ", name);
			Object& o = datas[current_block][name];

			if(std::holds_alternative<float>(o))
			{
				std::cout << " = " << std::get<float>(o) << "\n";
			}
			else if (std::holds_alternative<Troop>(o))
			{
				std::cout << " = Troop\n";
			}

			return o;
		}

		current_block--;
	}

	Debug::FatalError("UNDEFINED VARIABLE ", name);
	return Object{ std::monostate{} };
}

float Environment::GetAliveTroops()
{
	Debug::Log("GetAliveTroops Called\n");
	return 1.f;
}

float Environment::GetAliveTroopsInTeam(float team_id)
{
	Debug::Log("GetAliveTroopsInTeam Called : team_id = ", team_id, "\n");
	return team_id;
}

float Environment::GetElapsedTime()
{
	Debug::Log("GetElapsedTime Called\n");
	return 0.0f;
}

void Environment::Spawn(Troop troop, float x, float y, float team_id)
{
	Debug::Log("Spawn Called : x = ", x, ", y = ", y, ", team_id = ", team_id, "\n");
}

void Environment::SpawnMultiple(Troop troop, float x, float y, float amount, float team_id)
{
	Debug::Log("Spawn Called : x = ", x, ", y = ", y, ", amount = ", amount, ", team_id = ", team_id, "\n");
}

void Environment::TimeScale(float scale)
{
	Debug::Log("TimeScale Called : scale = ", scale, "\n");
}
