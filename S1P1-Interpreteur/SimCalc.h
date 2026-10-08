#pragma once
#include <vector>
#include "Types.h"

struct SimCalc
{
	std::vector<float> posX , posY ,healthpoints, damages, progress_atkspeeds, atkspeeds, speeds, ranges;
	std::vector<int> team_ids;

public:
	SimCalc();

	void UpdateSIMD(float dt);
	__declspec(noinline) void UpdateScalar(float dt);

	void UpdateAtkSpeedSIMD(float dt);
	__declspec(noinline) void UpdateAtkSpeedScalar(float dt);

	void UpdateLivingSIMD();
	__declspec(noinline) void UpdateLivingScalar();

	void UpdateMovingSIMD(std::vector<size_t>& indexes, float dt);
	__declspec(noinline) void UpdateMovingScalar(std::vector<size_t>& indexes, float dt);

	void TroopPrint();

	void UpdateAttackSIMD(std::vector<size_t>& indexes, float dt);
	__declspec(noinline) void UpdateAttackScalar(std::vector<size_t>& indexes, float dt);

	void AddSpawnTroop(SpawnTroop& sp, float x, float y, float team_id);
	void RemoveSpawnTroop(size_t index);

	float GetAliveTroops() const;
	float GetAliveTroopsInTeam(int team_id) const;

	float IsBattling();
	float GetWinnerTeam();
};

