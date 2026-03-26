#include "framework.h"

void Weapon::Cache(bool State) {
	CacheIndex(State);
	CacheWeaponArrayList(State);
	TraverseString(State);
	CacheMag(State);
}

void Weapon::CacheIndex(bool State) {
	if(State)
	m_Index = Coms->ReadVirtual<INT32>(m_Base + Offsets::WeaponIndex);
}

void Weapon::CacheWeaponArrayList(bool State) {
	if (State) {

		std::vector<WeaponArrayListEntry> List;

		auto ListLocation = AutoArray(m_Base + Offsets::WeaponListArray);

		int ArrayReadSize = 30; //using this as a abritrary number, need to find the actual size of the list aka the number of weapons.

		if (!ListLocation.Allocate(ArrayReadSize, 0x0, 0x28))
			return;

		for (int i = 0; i < ArrayReadSize; i++) {


			WeaponArrayListEntry Temp;
			Temp.WeaponMode = ListLocation.Get(i);
			Temp.WeaponType = ListLocation.Get(i,0x8);
			Temp.MuzzleType = ListLocation.Get(i,0x10);
			Temp.MuzzleState = ListLocation.Get(i,0x18);

			List.push_back(Temp);

		}
		m_List = List;
	}
}

void Weapon::TraverseString(bool State) {

	if (State) {
		if (m_Index >= 0x0 && m_Index < 0x3E8) {

			m_CurrentWeaponStruct = m_List[m_Index];

			UINT64 StringBuff = Coms->ReadVirtual<UINT64>(m_CurrentWeaponStruct.WeaponType + 0x30);

			//UINT64 StringBuff2 = Coms->ReadVirtual<UINT64>(StringBuff + 0x8);

			m_CurrWeapon = Coms->ReadString(StringBuff + 0x10);
		}

	}
}

void Weapon::CacheMag(bool State) {

	if (State) {
		m_Mag.Init(Coms->ReadVirtual<UINT64>(m_CurrentWeaponStruct.MuzzleState + 0x48));
	}
	m_Mag.Cache(State);

}