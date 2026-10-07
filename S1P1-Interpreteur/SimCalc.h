#pragma once
#include <vector>
#include "Types.h"

struct SimCalc
{
	std::vector<float> posX , posY ,healthpoints, damages, progress_atkspeeds, ranges;
	std::vector<int> team_ids;

	std::vector<SpawnTroop> spawn_troops;

public:
	SimCalc();

	void UpdateSIMD(float dt);
	void UpdateScalar(float dt);

	void UpdateAtkSpeedSIMD(float dt);
	void UpdateAtkSpeedScalar(float dt);

	void UpdateLivingSIMD();
	void UpdateLivingScalar();

	void TroopPrint();

	void UpdateAttackSIMD(std::vector<size_t>& indexes);
	void UpdateAttackScalar(std::vector<size_t>& indexes);

	void AddSpawnTroop(SpawnTroop& sp, float x, float y, float team_id);
	void RemoveSpawnTroop(size_t index);

	float GetAliveTroops() const;
	float GetAliveTroopsInTeam(int team_id) const;

	float IsBattling();
	float GetWinnerTeam();
};

