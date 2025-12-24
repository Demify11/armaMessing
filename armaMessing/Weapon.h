#pragma once

struct WeaponArrayListEntry {
	UINT64 WeaponMode;
	UINT64 WeaponType;
	UINT64 MuzzleType;
	UINT64 MuzzleState;
	UINT64 Buffer;
};

class Weapon
{
public:
	UINT64 m_Base;

private:
	std::vector<WeaponArrayListEntry> m_List;
	std::string m_CurrWeapon;

	Magazine m_Mag;
	INT32 m_Index;

public:
	void Cache(bool State);

private:
	void CacheMag(bool State);
	void CacheIndex(bool State);
	void CacheWeaponArrayList(bool State);

	void TraverseString(bool State);

};

