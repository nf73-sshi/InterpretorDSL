#include "SimCalc.h"
#include <random>
#include <iostream>
#include <immintrin.h> 

SimCalc::SimCalc()
{
}

void SimCalc::UpdateSIMD(float dt)
{
	UpdateAtkSpeedSIMD(dt);
	UpdateLivingSIMD();
}

__declspec(noinline) void SimCalc::UpdateScalar(float dt)
{
	UpdateAtkSpeedScalar(dt);
	UpdateLivingScalar();
}

void SimCalc::UpdateAtkSpeedSIMD(float dt)
{
	size_t i = 0;
	size_t size = progress_atkspeeds.size();

	__m128 deltaTime = _mm_set_ps1(dt);

	std::vector<size_t> indexes_to_atk; //tab with troops which can attack
	std::vector<size_t> indexes_to_move;

	for (; i + 3 < size; i += 4)
	{
		__m128 AS = _mm_loadu_ps(&progress_atkspeeds[i]);
		__m128 sub = _mm_sub_ps(AS , deltaTime);

		_mm_store_ps(&progress_atkspeeds[i], sub);

		//check if can attack
		__m128 m_mask = _mm_cmple_ps(sub, _mm_setzero_ps());
		int mask = _mm_movemask_ps(m_mask);

		if (mask != 0)
		{
			if (mask & 1) // 0001
				indexes_to_atk.push_back(i + 0);
			else
				indexes_to_move.push_back(i + 0);

			if (mask & 2) // 0010
				indexes_to_atk.push_back(i + 1);
			else
				indexes_to_move.push_back(i + 1);

			if (mask & 4) // 0100
				indexes_to_atk.push_back(i + 2);
			else
				indexes_to_move.push_back(i + 2);

			if (mask & 8) // 1000
				indexes_to_atk.push_back(i + 3);
			else
				indexes_to_move.push_back(i + 4);
		}
	}
	for (; i < size; ++i)
	{
		progress_atkspeeds[i] -= dt;

		if (progress_atkspeeds[i] <= 0)
			indexes_to_atk.push_back(i);
		else
			indexes_to_move.push_back(i);
	}

	UpdateAttackSIMD(indexes_to_atk, dt);
	UpdateMovingSIMD(indexes_to_move, {}, {}, dt);
}

__declspec(noinline) void SimCalc::UpdateAtkSpeedScalar(float dt)
{
	size_t size = progress_atkspeeds.size();
	std::vector<size_t> indexes_to_atk;
	std::vector<size_t> indexes_to_move;

	#pragma loop(no_vector)
	for (size_t i = 0; i < size; ++i)
	{
		progress_atkspeeds[i] -= dt;

		if (progress_atkspeeds[i] <= 0)
			indexes_to_atk.push_back(i);
		else
			indexes_to_move.push_back(i);
	}

	UpdateAttackScalar(indexes_to_atk, dt);
	UpdateMovingScalar(indexes_to_move, {}, {}, dt);
}

void SimCalc::UpdateLivingSIMD()
{
	size_t size = team_ids.size();
	size_t i = 0;
	std::vector<size_t> indexes_to_erase = {};

	for (; i + 3 < size; i += 4)
	{
		__m128 hps = _mm_loadu_ps(&healthpoints[i]);

		//check if alive
		__m128 m_mask = _mm_cmple_ps(hps, _mm_setzero_ps());
		int mask = _mm_movemask_ps(m_mask);

		if (mask != 0)
		{
			if (mask & 1) // 0001
				indexes_to_erase.push_back(i + 0);

			if (mask & 2) // 0010
				indexes_to_erase.push_back(i + 1);

			if (mask & 4) // 0100
				indexes_to_erase.push_back(i + 2);

			if (mask & 8) // 1000
				indexes_to_erase.push_back(i + 3);
		}
	}
	for (; i < size; ++i)
	{
		if (healthpoints[i] <= 0)
		{
			indexes_to_erase.push_back(i);
		}
	}

	for (size_t j = indexes_to_erase.size(); j > 0; --j)
	{
#ifdef PRINT_DEBUG
		std::cout << "Troop[" << team_ids[indexes_to_erase[j - 1]] << "] " << unique_ids_name[indexes_to_erase[j-1]] << " is DEAD !" << std::endl;
#endif
		RemoveSpawnTroop(indexes_to_erase[j - 1]);
	}
}

__declspec(noinline) void SimCalc::UpdateLivingScalar()
{
	size_t size = team_ids.size();
	std::vector<size_t> indexes_to_erase = {};

	#pragma loop(no_vector)
	for (size_t i = 0; i < size; ++i)
	{
		if (healthpoints[i] <= 0)
		{
			indexes_to_erase.push_back(i);
		}
	}

	for (size_t i = indexes_to_erase.size(); i > 0; --i)
	{
#ifdef PRINT_DEBUG
		std::cout << "Troop[" << team_ids[indexes_to_erase[i - 1]] << "] " << unique_ids_name[indexes_to_erase[i - 1]] << " is DEAD !" << std::endl;
#endif
		RemoveSpawnTroop(indexes_to_erase[i - 1]);
	}
}

void SimCalc::UpdateMovingSIMD(std::vector<size_t>& indexes, std::vector<float> dxs, std::vector<float> dys, float dt)
{
	size_t indexes_size = indexes.size();
	size_t size = team_ids.size();

	if (dxs.empty() || dys.empty()) //Find the enemy
	{
		for (size_t i = 0; i < indexes_size; ++i)
		{
			bool hasRange = false;

			float minDistX = -1;
			float minDistY = -1;

			size_t attacker_i = indexes[i];

			size_t target_index = size_t(-1);
			float minValue = -1;
			__m128 m_posX1 = _mm_set_ps1(posX[attacker_i]);
			__m128 m_posY1 = _mm_set_ps1(posY[attacker_i]);

			size_t j = 0;
			for (; j + 3 < size; j += 4) //4 tests 
			{
				__m128 m_posX2 = _mm_loadu_ps(&posX[j]);
				__m128 m_posY2 = _mm_loadu_ps(&posY[j]);

				__m128 m_dx = _mm_sub_ps(m_posX2, m_posX1); // x1 - x2
				__m128 m_sqr_dist = _mm_mul_ps(m_dx, m_dx); // dx * dx

				__m128 m_dy = _mm_sub_ps(m_posY2, m_posY1); // y1 - y2
				m_sqr_dist = _mm_add_ps(m_sqr_dist, _mm_mul_ps(m_dy, m_dy)); // + dy * dy

				float sqr_dists[4];
				_mm_storeu_ps(sqr_dists, m_sqr_dist);

				for (size_t k = 0; k < 4; ++k)
				{
					if (team_ids[attacker_i] == team_ids[j + k] || healthpoints[j + k] <= 0)
						continue;

					if (minValue > sqr_dists[k] || minValue == -1)
					{
						float _dxs[4], _dys[4];
						_mm_storeu_ps(_dxs, m_dx); _mm_storeu_ps(_dys, m_dy);

						minDistX = _dxs[k];
						minDistY = _dys[k];

						if (sqr_dists[k] > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
						{
							hasRange = false;
							continue;
						}

						hasRange = true;

						minValue = sqr_dists[k];
						target_index = j + k;
					}
				}
			}
			for (; j < size; j++)
			{
				if (team_ids[attacker_i] == team_ids[j] || healthpoints[j] <= 0)
					continue;

				float distX = posX[j] - posX[attacker_i];
				float distY = posY[j] - posY[attacker_i];

				float sqrDist = distX * distX + distY * distY;

				if (minValue > sqrDist || minValue == -1)
				{
					minDistX = distX;
					minDistY = distY;

					if (sqrDist > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
					{
						hasRange = false;
						continue;
					}

					hasRange = true;
			
					minValue = sqrDist;
					target_index = j;
				}
			}

			if (target_index == size_t(-1) || hasRange)
			{
				dxs.push_back(0);
				dys.push_back(0);
			}
			else
			{
				dxs.push_back(minDistX);
				dys.push_back(minDistY);
			}
		}
	}

	for (size_t i = 0; i < indexes_size; ++i) //MOVE
	{
		float dx = dxs[i];
		float dy = dys[i];
		if (dx == 0 && dy == 0)
			continue;

		float norm = sqrt(dx * dx + dy * dy);

		posX[indexes[i]] += dx / norm * dt;
		posY[indexes[i]] += dy / norm * dt;
	}
}

void SimCalc::UpdateMovingScalar(std::vector<size_t>& indexes, std::vector<float> dxs, std::vector<float> dys, float dt)
{
	size_t indexes_size = indexes.size();
	size_t size = team_ids.size();

	if (dxs.empty() || dys.empty()) //Find the enemy
	{
		#pragma loop(no_vector)
		for (size_t i = 0; i < indexes_size; ++i)
		{
			bool hasRange = false;

			float minDistX = -1;
			float minDistY = -1;

			size_t attacker_i = indexes[i];
			size_t target_index = -1;
			float minValue = -1;

			#pragma loop(no_vector)
			for (size_t j = 0; j < size; j++)
			{
				if (team_ids[attacker_i] == team_ids[j] || healthpoints[j] <= 0)
					continue;

				float distX = posX[j] - posX[attacker_i];
				float distY = posY[j] - posY[attacker_i];

				float sqrDist = distX * distX + distY * distY;

				if (minValue > sqrDist || minValue == -1)
				{
					minDistX = distX;
					minDistY = distY;

					if (sqrDist > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
					{
						hasRange = false;
						continue;
					}

					hasRange = true;

					minValue = sqrDist;
					target_index = j;
				}
			}

			if (target_index == size_t(-1) || hasRange)
			{
				dxs.push_back(0);
				dys.push_back(0);
			}
			else
			{
				dxs.push_back(minDistX);
				dys.push_back(minDistY);
			}
		}
	}
	
	for (size_t i = 0; i < indexes_size; ++i) //MOVE
	{
		float dx = dxs[i];
		float dy = dys[i];
		if (dx == 0 && dy == 0)
			continue;

		float norm = sqrt(dx * dx + dy * dy);


		posX[indexes[i]] += dx / norm * dt;
		posY[indexes[i]] += dy / norm * dt;
	}
}

void SimCalc::TroopPrint()
{
	for (int i = 0; i < team_ids.size(); i++)
	{
		std::cout << "Troop[" << team_ids[i] << "] " << unique_ids_name[i] << " Pos(" << posX[i] << ", " << posY[i] << ")  Hp: " << healthpoints[i] << std::endl;
	}
	std::cout << "\n";
}

void SimCalc::UpdateAttackSIMD(std::vector<size_t>& indexes, float dt)
{
	std::vector<size_t> indexes_cant_attack;
	std::vector<float> minDistXs;
	std::vector<float> minDistYs;

	size_t attackers_size = indexes.size();
	size_t size = team_ids.size();

	for (size_t i = 0; i < attackers_size; ++i)
	{
		bool hasRange = false;

		float minDistX = -1;
		float minDistY = -1;

		size_t attacker_i = indexes[i];

		size_t target_index = size_t(-1);
		float minValue = -1;
		__m128 m_posX1 = _mm_set_ps1(posX[attacker_i]);
		__m128 m_posY1 = _mm_set_ps1(posY[attacker_i]);

		size_t j = 0;
		for (; j + 3 < size; j += 4) //4 tests 
		{
			__m128 m_posX2 = _mm_loadu_ps(&posX[j]);
			__m128 m_posY2 = _mm_loadu_ps(&posY[j]);

			__m128 m_dx = _mm_sub_ps(m_posX2, m_posX1); // x1 - x2
			__m128 m_sqr_dist = _mm_mul_ps(m_dx, m_dx); // dx * dx

			__m128 m_dy = _mm_sub_ps(m_posY2, m_posY1); // y1 - y2
			m_sqr_dist = _mm_add_ps(m_sqr_dist, _mm_mul_ps(m_dy, m_dy)); // + dy * dy

			float sqr_dists[4];
			_mm_storeu_ps(sqr_dists, m_sqr_dist);

			for (size_t k = 0; k < 4; ++k)
			{
				if (team_ids[attacker_i] == team_ids[j+k] || healthpoints[j+k] <= 0)
					continue;

				if (minValue > sqr_dists[k] || minValue == -1)
				{
					float dxs[4], dys[4];
					_mm_storeu_ps(dxs, m_dx); _mm_storeu_ps(dys, m_dy);

					minDistX = dxs[k];
					minDistY = dys[k];

					if (sqr_dists[k] > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
					{
						hasRange = false;
						continue;
					}

					hasRange = true;

					minValue = sqr_dists[k];
					target_index = j + k;
				}
			}
		}
		for (; j < size; ++j)
		{
			if (team_ids[attacker_i] == team_ids[j] || healthpoints[j] <= 0)
				continue;

			float distX = posX[j] - posX[attacker_i];
			float distY = posY[j] - posY[attacker_i];

			float sqrDist = distX * distX + distY * distY;

			if (minValue > sqrDist || minValue == -1)
			{
				minDistX = distX;
				minDistY = distY;

				if (sqrDist > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
				{
					hasRange = false;
					continue;
				}

				hasRange = true;

				minValue = sqrDist;
				target_index = j;
			}
		}

		if (target_index == size_t(-1))
		{
			if (hasRange == false)
			{
				indexes_cant_attack.push_back(attacker_i);
				minDistXs.push_back(minDistX);
				minDistYs.push_back(minDistY);
			}

			continue;
		}

		healthpoints[target_index] -= damages[attacker_i];
		progress_atkspeeds[attacker_i] = atkspeeds[attacker_i];


#ifdef PRINT_DEBUG
		std::cout << "Troop[" << team_ids[attacker_i] << "] " << unique_ids_name[attacker_i] << " Pos(" << posX[attacker_i] << ", " << posY[attacker_i] << ") ATTACKS Troop[" << team_ids[attacker_i] <<
		"] " << unique_ids_name[target_index] << " Pos(" << posX[target_index] << ", " << posY[target_index] << ") Hp Now : " << healthpoints[target_index] << std::endl << std::endl;
#endif

	}

	UpdateMovingSIMD(indexes_cant_attack, minDistXs, minDistYs, dt);
}

__declspec(noinline) void SimCalc::UpdateAttackScalar(std::vector<size_t>& indexes, float dt)
{
	std::vector<size_t> indexes_cant_attack;
	std::vector<float> minDistXs;
	std::vector<float> minDistYs;

	size_t attackers_size = indexes.size();
	size_t size = team_ids.size();

	#pragma loop(no_vector)
	for (size_t i = 0; i < attackers_size; ++i)
	{
		bool hasRange = false;

		float minDistX = -1;
		float minDistY = -1;

		size_t attacker_i = indexes[i];
		size_t target_index = -1;
		float minValue = -1;

		#pragma loop(no_vector)
		for (size_t j = 0; j < size; j++)
		{
			if (team_ids[attacker_i] == team_ids[j] || healthpoints[j] <= 0)
				continue;

			float distX = posX[j] - posX[attacker_i];
			float distY = posY[j] - posY[attacker_i];

			float sqrDist = distX * distX + distY * distY;

			if (minValue > sqrDist || minValue == -1)
			{
				minDistX = distX;
				minDistY = distY;

				if (sqrDist > ranges[attacker_i] * ranges[attacker_i]) // if the attacker hasn't enough range
				{
					hasRange = false;
					continue;
				}

				hasRange = true;

				minValue = sqrDist;
				target_index = j;
			}
		}

		if (target_index == size_t(-1))
		{
			if (hasRange == false)
			{
				indexes_cant_attack.push_back(attacker_i);
				minDistXs.push_back(minDistX);
				minDistYs.push_back(minDistY);
			}

			continue;
		}

		healthpoints[target_index] -= damages[attacker_i];
		progress_atkspeeds[attacker_i] = atkspeeds[attacker_i];

#ifdef PRINT_DEBUG
		std::cout << "Troop[" << team_ids[attacker_i] << "] " << unique_ids_name[attacker_i] << " Pos(" << posX[attacker_i] << ", " << posY[attacker_i] << ") ATTACKS Troop[" << team_ids[target_index] <<
			"] " << unique_ids_name[target_index] << " Pos(" << posX[target_index] << ", " << posY[target_index] << ") Hp Now : " << healthpoints[target_index] << std::endl << std::endl;
#endif
	}

	UpdateMovingScalar(indexes_cant_attack, minDistXs, minDistYs, dt);
}

void SimCalc::AddSpawnTroop(SpawnTroop& sp, float x, float y, float team_id)
{
	unique_ids_name.push_back(current_name_id++);

	posX.push_back(x);
	posY.push_back(y);

	healthpoints.push_back(sp.hp);
	damages.push_back(sp.dmg);

	progress_atkspeeds.push_back(0);
	atkspeeds.push_back(sp.atkspeed);

	speeds.push_back(sp.speed);
	ranges.push_back(sp.range);

	team_ids.push_back(team_id);
}

void SimCalc::RemoveSpawnTroop(size_t index)
{
	std::swap(unique_ids_name[index], unique_ids_name.back());
	unique_ids_name.pop_back();

	std::swap(posX[index], posX.back());
	posX.pop_back();

	std::swap(posY[index], posY.back());
	posY.pop_back();

	std::swap(healthpoints[index], healthpoints.back());
	healthpoints.pop_back();

	std::swap(damages[index], damages.back());
	damages.pop_back();

	std::swap(progress_atkspeeds[index], progress_atkspeeds.back());
	progress_atkspeeds.pop_back();

	std::swap(atkspeeds[index], atkspeeds.back());
	atkspeeds.pop_back();

	std::swap(speeds[index], speeds.back());
	speeds.pop_back();

	std::swap(ranges[index], ranges.back());
	ranges.pop_back();

	std::swap(team_ids[index], team_ids.back());
	team_ids.pop_back();
}

float SimCalc::GetAliveTroops()
{
	std::cout << "There are " << team_ids.size() << " troops alive\n";

	return static_cast<float>(team_ids.size());
}

float SimCalc::GetAliveTroopsInTeam(int team_id) const
{
	if (team_ids.empty())
		return 0.0f;

	size_t count = 0;
	size_t size = team_ids.size();
	size_t i = 0;

	__m128i m_ref = _mm_set1_epi32(team_id);

	for (; i + 3 < size; i += 4)
	{
		__m128i m_team_ids = _mm_loadu_epi32((const void*)&team_ids[i]);
		__m128i m_mask = _mm_cmpeq_epi32(m_team_ids, m_ref);

		int mask = _mm_movemask_ps(_mm_castsi128_ps(m_mask)); //cast m128i in m128 to movemask_ps

		if (mask != 0)
		{
			if (mask & 1) // 0001
				count++;

			if (mask & 2) // 0010
				count++;

			if (mask & 4) // 0100
				count++;

			if (mask & 8) // 1000
				count++;
		}
	}
	for (; i < size; ++i)
	{
		if (team_ids[i] == team_id)
			count++;
	}

	std::cout << "There are " << count << " troops alive in team " << team_id << "\n";
	return static_cast<float>(count);
}

float SimCalc::IsBattling()
{
	if (team_ids.empty())
		return 0.f;

	bool more_than_one_team = false;

	size_t size = team_ids.size();
	size_t i = 0;

	int team_id_ref = team_ids[0];
	__m128i m_ref = _mm_set1_epi32(team_id_ref);

	for (; i + 3 < size; i += 4)
	{
		__m128i m_team_ids = _mm_loadu_epi32((const void*)&team_ids[i]);
		__m128i m_mask = _mm_cmpeq_epi32(m_team_ids, m_ref);

		int mask = _mm_movemask_ps(_mm_castsi128_ps(m_mask)); //cast m128i in m128 to movemask_ps

		if (mask != 0b1111) //if one is not on the same team
		{
			more_than_one_team = true;
			break;
		}
	}

	for (; i < size; ++i)
	{
		if (team_ids[i] != team_id_ref)
		{
			more_than_one_team = true;
			break;
		}
	}

	return static_cast<float>(more_than_one_team);
}

float SimCalc::GetWinnerTeam()
{
	if (IsBattling())
		return -1;

	if (team_ids.empty())
	{
		std::cout << "DRAW !\n";
		return -1;
	}
	else
	{
		int winner = team_ids[0];

		std::cout << "TEAM " << winner << " WINS !\n";

		return winner;
	}
}
