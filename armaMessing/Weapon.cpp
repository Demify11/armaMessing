#include "framework.h"

void Weapon::Cache(bool State) {

	if (!m_Base)
		return;

	CacheIndex(State);
	CacheWeaponArrayList(State);
	TraverseString(State);
	CacheWeaponStruct(State);
	CacheMag(State);
	//FindAttachment(State);
	CacheZeroing(State);

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

			if (m_List.size() <= m_Index)
				return;	// f.

			m_CurrentWeaponStruct = m_List[m_Index];

			UINT64 StringBuff = Coms->ReadVirtual<UINT64>(m_CurrentWeaponStruct.WeaponType + 0x30);

			//UINT64 StringBuff2 = Coms->ReadVirtual<UINT64>(StringBuff + 0x8);

			m_CurrWeapon = Coms->ReadString(StringBuff + 0x10);
		}

	}
}

void Weapon::CacheWeaponStruct(bool State) {

	m_InitSpeed = Coms->ReadVirtual<float>(m_CurrentWeaponStruct.WeaponType + 0x4F4);


}

void Weapon::CacheMag(bool State) {

	if (State) {
		m_Mag.Init(Coms->ReadVirtual<UINT64>(m_CurrentWeaponStruct.MuzzleType + 0x5E8));
		m_Mag.Cache(State);
	}
	

}

float Weapon::FindAttachment(bool State) {
	if (State) {
		float err = 0;
		UINT64 CurrentSlotDataEntry = 0;
		

		auto MuzzleType = Coms->ReadVirtual<INT32>(m_CurrentWeaponStruct.MuzzleType + 0x268);

		auto SlotsManager = Coms->ReadVirtual<UINT64>(m_Base + 0xD58);

		auto Buffer = Coms->ReadVirtual<UINT64>(SlotsManager + 0x20);
		m_SlotCount = Coms->ReadVirtual<INT32>(Buffer + 0x20);

		auto SlotData = AutoArray(Buffer + 0x18);

		if (!SlotData.Allocate((m_SlotCount * 0x8), 0x0, 0x8))
			return err;

		for (int i = 0; i < m_SlotCount; i++) {

			auto SlotDataEntry = SlotData.Get(i);
			auto OwnerWeaponType = Coms->ReadVirtual<UINT64>(SlotDataEntry + 0x38);

			if (OwnerWeaponType == m_CurrentWeaponStruct.WeaponType) {
				CurrentSlotDataEntry = SlotDataEntry;
			}

		}

		auto WeaponInstance = CurrentSlotDataEntry;

		if (WeaponInstance) {

			auto OpticItemSlot = Coms->ReadVirtual<UINT64>(WeaponInstance + 0x88);

			auto Mode = Coms->ReadVirtual<INT32>(OpticItemSlot + 0x118); //this is wether we are using top or bottom scope

			auto OpticState = OpticItemSlot + 0x110;

			auto ZeroingStructLoc = Coms->ReadVirtual<UINT64>(OpticItemSlot + 0xC8);

			WeaponZeroingStruct ZeroingStruct = Coms->ReadVirtual<WeaponZeroingStruct>(ZeroingStructLoc + (0x148 * Mode));

			float Zoom = Coms->ReadVirtual<float>(m_Base + 0xF10);

			if (!ZeroingStruct.discreteCount) {

				if (Zoom > ZeroingStruct.opticsZoomMin) {

					if (Zoom >= ZeroingStruct.opticsZoomMax) {

						return ZeroingStruct.distanceZoomMax;

					}

					float ret = ((Zoom - ZeroingStruct.opticsZoomMin) / (ZeroingStruct.opticsZoomMax - ZeroingStruct.opticsZoomMin) *
						(ZeroingStruct.distanceZoomMax - ZeroingStruct.distanceZoomMin) + ZeroingStruct.distanceZoomMin);

					return ret;

				}

				return ZeroingStruct.distanceZoomMin;

			}

			int CurrentStep = Coms->ReadVirtual<int>(OpticState + 0x8);

			UINT64 StepTable = Coms->ReadVirtual<UINT64>(OpticState + 0x10);

			UINT64 FieldAddr = StepTable + (UINT64)CurrentStep * 0x28 + 0xC;

			int StepMaxIndex = Coms->ReadVirtual<int>(FieldAddr);

			int ZeroIndex = ZeroingStruct.discreteCount - 1;
			if (StepMaxIndex < ZeroIndex)
				ZeroIndex = StepMaxIndex;

			auto DiscreteDistance = Coms->ReadVirtual<float>(ZeroingStruct.discreteList + (UINT64)ZeroIndex * 4);

			return DiscreteDistance;

			

		}

		return err;
	}

	
}

void Weapon::CacheZeroing(bool State) {
	if (!State)
		return;


	if (!m_Resolved || m_Index != m_LastIndex) { //Checking weapon Change
		m_HasOptic = ResolveOptic();
		m_Resolved = true;
		m_LastIndex = m_Index;
		m_LastCap = -1;
		m_LastZoom = -1.0f;
		if (!m_HasOptic) {
			m_Zeroing = 0.0f; 
			m_IsZeroed = false;
			return;
		}
	}

	int OpticIndex = Coms->ReadVirtual<int>(m_OpticItemSlot + 0x118);

	if (OpticIndex != m_LastMode) { //Checking Weapon Scope Change
		m_HasOptic = ResolveOptic();
		m_LastCap = -1;
		m_LastZoom = -1.0f;
		if (!m_HasOptic) { 
			m_Zeroing = 0.0f;
			m_IsZeroed = false;
			return;
		}
		OpticIndex = m_LastMode;   // ResolveOptic refreshed it
	}


	if (m_ZeroingStruct.discreteCount == 0) {

		float Zoom = Coms->ReadVirtual<float>(m_Base + 0xF10);
		if (m_IsZeroed && Zoom == m_LastZoom)
			return;

		m_LastZoom = Zoom;

		if (Zoom > m_ZeroingStruct.opticsZoomMin) {

			if (Zoom >= m_ZeroingStruct.opticsZoomMax) {

				m_Zeroing = m_ZeroingStruct.distanceZoomMax;
			}
			else {

				m_Zeroing = (Zoom - m_ZeroingStruct.opticsZoomMin) / (m_ZeroingStruct.opticsZoomMax - m_ZeroingStruct.opticsZoomMin)
					* (m_ZeroingStruct.distanceZoomMax - m_ZeroingStruct.distanceZoomMin) + m_ZeroingStruct.distanceZoomMin;
			}
		}
		else {

			m_Zeroing = m_ZeroingStruct.distanceZoomMin;

		}

		m_IsZeroed = true;
		return;
	}


	UINT64 FieldAddr = m_StepTable + (UINT64)OpticIndex * 0x28 + 0xC;
	int StepMaxIndex = Coms->ReadVirtual<int>(FieldAddr);// Checking Zoom NotchChange

	if (m_IsZeroed && StepMaxIndex == m_LastCap)
		return; // nothing scrolled

	m_LastCap = StepMaxIndex;

	int ZeroIndex = m_ZeroingStruct.discreteCount - 1;
	if (StepMaxIndex < ZeroIndex)
		ZeroIndex = StepMaxIndex; // signed clamp viewable in ida

	m_Zeroing = Coms->ReadVirtual<float>(m_ZeroingStruct.discreteList + (UINT64)ZeroIndex * 4);

	m_IsZeroed = true;
}

bool Weapon::ResolveOptic() {
	UINT64 CurrentSlotDataEntry = 0;

	auto SlotsManager = Coms->ReadVirtual<UINT64>(m_Base + 0xD58);
	auto Buffer = Coms->ReadVirtual<UINT64>(SlotsManager + 0x20);
	m_SlotCount = Coms->ReadVirtual<INT32>(Buffer + 0x20);

	auto SlotData = AutoArray(Buffer + 0x18);
	if (!SlotData.Allocate((m_SlotCount * 0x8), 0x0, 0x8))
		return false;

	for (int i = 0; i < m_SlotCount; i++) {
		auto SlotDataEntry = SlotData.Get(i);
		//SlotDataEntry doesnt actually change on mode toggle so we could cache this as well.
		//E.g When the mode of the weapon is changed we dont need to recache the SlotData, We can make a function for Chaching slots
		//and a function for Caching the ModeStruct, which needs updating on anychange, SlotData only needs updating on scope/weapon change.
		auto OwnerWeaponType = Coms->ReadVirtual<UINT64>(SlotDataEntry + 0x38);

		if (OwnerWeaponType == m_CurrentWeaponStruct.WeaponType) {

			CurrentSlotDataEntry = SlotDataEntry;
			break;
		}

	}

	auto WeaponInstance = CurrentSlotDataEntry;
	if (!WeaponInstance)
		return false;

	m_OpticItemSlot = Coms->ReadVirtual<UINT64>(WeaponInstance + 0x88);

	int OpticIndex = Coms->ReadVirtual<INT32>(m_OpticItemSlot + 0x118);
	m_LastMode = OpticIndex;

	auto ZeroingStructLoc = Coms->ReadVirtual<UINT64>(m_OpticItemSlot + 0xC8);
	m_ZeroingStruct = Coms->ReadVirtual<WeaponZeroingStruct>(ZeroingStructLoc + (0x148 * OpticIndex));

	if (m_ZeroingStruct.discreteCount != 0) {

		UINT64 OpticState = m_OpticItemSlot + 0x110;
		m_StepTable = Coms->ReadVirtual<UINT64>(OpticState + 0x10);

	}

	return true;
}