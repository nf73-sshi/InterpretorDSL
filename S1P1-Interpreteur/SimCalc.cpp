#include "SimCalc.h"
#include <random>
#include <iostream>
SimCalc::SimCalc(std::vector<Troop*> troops)
{
	std::srand(std::time(0));
	{ troopsList = troops; }
	CreateAllInstance();
	CreateRandomPosition();
}

void SimCalc::CreateRandomPosition()
{
	for (int i = 0; i < spawnTroopList.size(); i++)
	{
		posX.push_back(std::rand() % 10);
		posY.push_back(std::rand() % 10);
	}
}

void SimCalc::Update()
{
	UpdateAttackSpeed();
	TroopPrint();

}

void SimCalc::UpdateAttackSpeed()
{
	size_t i = 0;
	size_t size = attackSpeedList.size();

	__m128 deltaTime = _mm_set1_ps(dt);

	for (; i + 4 <= size; i += 4)
	{
		__m128 AS = _mm_loadu_ps(&attackSpeedList[i]);

		__m128 sub = _mm_sub_ps(AS , deltaTime);

		_mm_store_ps(&attackSpeedList[i], sub);
	}
	for (; i < size; ++i)
	{
		attackSpeedList[i] -= dt;
	}
	std::vector<int> indexes;

	for (int x = 0; x < size; x++)
	{
		if (attackSpeedList[x] >= 0)
		{
			indexes.push_back(x);
		}
	}
	TroopAttack(indexes);

	

}

void SimCalc::TroopPrint()
{
	for (int i = 0; i < spawnTroopList.size(); i++)
	{
		std::cout << "Troop : " << i << " Hp : " << hpList[i] << " X : " << posX[i] << " Y : " << posY[i] << std::endl;
	}
}

void SimCalc::TroopAttack(std::vector<int>& indexes)
{
	std::vector<int> ennemiIndex;

	for (int index : indexes)
	{
		float valueIndex = 0;
		float minValue = -1;
		for (int i = 0; i < posX.size(); i++)
		{
			if (i == index)
				continue;
			float distX = posX[index] - posX[i];
			float distY = posY[index] - posY[i];	

			float Value = distX * distX + distY * distY;

			if (minValue == -1)
			{
				minValue = distX + distY;
				valueIndex = i;
			}
			else if (minValue > Value)
			{
				minValue = Value;
				valueIndex = i;
			}
		}
		ennemiIndex.push_back(valueIndex);
	}

	for (int i = 0;i < indexes.size();i++)
	{
		std::cout << "Troop " << indexes[i] << " attaque Troop " << ennemiIndex[i] <<std::endl;
		hpList[ennemiIndex[i]] -= dmgList[indexes[i]];
		attackSpeedList[indexes[i]] = spawnTroopList[indexes[i]].baseAtkspeed;
	}
}

void SimCalc::CreateAllInstance()
{
	for (Troop* t : troopsList)
	{
		SpawnTroop  st;
		st.hp = t->hp;
		hpList.push_back(st.hp);
		//radiusList.push_back(t->radius);
		st.dmg = t->dmg;
		dmgList.push_back(st.dmg);
		st.baseAtkspeed = t->atkspeed;
		attackSpeedList.push_back(t->fatkspeed);

		spawnTroopList.push_back(st);
	}
}
