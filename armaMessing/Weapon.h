#pragma once

struct WeaponArrayListEntry {
	UINT64 WeaponMode;
	UINT64 WeaponType;
	UINT64 MuzzleType;
	UINT64 MuzzleState;
	UINT64 Buffer;
};

struct WeaponZeroingStruct {
	char _pad00[64];
	float opticsZoomMin;
	float opticsZoomMax;
	char _pad48[4];
	float distanceZoomMin;
	float distanceZoomMax;
	char _pad54[164];
	UINT64 discreteList;
	int discreteCount;
	char _pad104[68];
};

class Weapon {
public:
	UINT64 m_Base;
	float m_InitSpeed;
	Magazine m_Mag;
	float m_Zeroing;

private:
	std::vector<WeaponArrayListEntry> m_List;
	WeaponArrayListEntry m_CurrentWeaponStruct;
	std::string m_CurrWeapon;

	INT32 m_SlotCount;
	INT32 m_Index;

	bool m_IsZeroed = 0;
	bool m_Resolved = false;
	bool m_HasOptic = false;
	int m_LastIndex = -1;
	int m_LastMode = -1;
	UINT64 m_OpticItemSlot = 0;
	UINT64 m_StepTable = 0;
	WeaponZeroingStruct m_ZeroingStruct{};


	int m_LastCap = -1;
	float m_LastZoom = -1.0f;

	bool ResolveOptic();
	void CacheZeroing(bool State);
public:
	void Cache(bool State);

private:

	void CacheWeaponStruct(bool State);
	void CacheMag(bool State);
	float FindAttachment(bool State);
	void CacheIndex(bool State);
	void CacheWeaponArrayList(bool State);
	void TraverseString(bool State);

};

