#include "framework.h"

void Weapon::Cache(bool State) {
	CacheIndex(State);
	CacheWeaponArrayList(State);
	TraverseString(State);
}

void Weapon::CacheIndex(bool State) {
	if(State)
	m_Index = Coms->ReadVirtual<INT32>(m_Base + Offsets::Weapon::WeaponIndex);
}

void Weapon::CacheWeaponArrayList(bool State) {
	if (State) {

		std::vector<WeaponArrayListEntry> List;

		auto ListLocation = AutoArray(m_Base + Offsets::Weapon::WeaponArrayList);

		int ArrayReadSize = 25; //using this as a abritrary number, need to find the actual size of the list aka the number of weapons.

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
		WeaponArrayListEntry CurrentWeaponStruct = m_List[m_Index];

		UINT64 StringBuff = Coms->ReadVirtual<UINT64>(CurrentWeaponStruct.WeaponType + 0x20);
		UINT64 StringBuff2 = Coms->ReadVirtual<UINT64>(StringBuff + 0x8);
		m_CurrWeapon = Coms->ReadString(StringBuff2 + 0x10);

	}
}