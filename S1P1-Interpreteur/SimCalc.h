#pragma once
#include <vector>
#include "Types.h"

struct SimCalc
{
	std::vector<float> posX , posY ,hpList, radiusList, dmgList ,attackSpeedList;
	std::vector<int> id;
	std::vector<Troop*> troopsList;
	std::vector<SpawnTroop> spawnTroopList;
	float dt = 0.0016f;
public:
	SimCalc(std::vector<Troop*> troops);
	void CreateRandomPosition();
	void Update();
	void UpdateAttackSpeed();
	void TroopPrint();
	void TroopAttack(std::vector<int>& indexes);
	void CreateAllInstance();
};

