#pragma once
#include <variant>

enum Type
{
	TYPE_VOID,
	TYPE_TROOP,
	TYPE_FLOAT
};

struct alignas(32) Troop
{
	float cost = 0;
	float hp = 0;
	float radius = 0;
	float dmg = 0;

	float fatkspeed = 0;
	float atkspeed = 0;
	float speed = 0;
	float range = 0;
};

using Object = std::variant<std::monostate, float, Troop>;