#include "framework.h"

void Entity::Cache(bool State) {	// FOR EACH ENTITY YOU DO 3 READS - 100 * 3
	CacheVisualState(State);	// 2 read
//	CacheHeadPosition(State);	// 0 read
//	CacheFeetPosition (State);	// 0 read
//	CacheHeadPosition2 (State);
//	CacheGunAngles(State);	// <- this doesnt need to be cached if not local player!!!
	CacheDead(State);			// 1 read

}

void Entity::CacheDead(bool State) {
	dead = !(Coms->ReadVirtual<UINT8>(m_Base + 0x5CC) & 1);
}

void Entity::CacheFeetPosition(bool State) {
	FeetPos = Coms->ReadVirtual<Vector3>(VisualState + 0x2C);
}

void Entity::CacheVisualState(bool State) {
	
	if (State) {
		VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x180);
	}

	if (!VisualState)
		return;

	struct PageRead {
		char pad_0000[0x200];
	};

	auto Data = Coms->ReadVirtual<PageRead>(VisualState);

	FeetPos = *(Vector3*)(Data.pad_0000 + 0x2C);
	HeadPos = *(Vector3*)(Data.pad_0000 + 0x168);
}

void Entity::CacheHeadPosition2(bool State) {
	//const auto VisualState = Coms->Read(Address + 0xD0);
	const auto World = g_Client->m_World.GetBase();
	//Coms->Read<UINT64>(ModuleBase + 0x2674500);	// camera is only at local player.
	const auto Camera = Coms->ReadVirtual<UINT64>(World + 0xD30);
	
	// this visual state is "laggier" for other entities. so use the other one. +D0
	// but for local player, it's the only one that works + its fast on local player
	// trial and error + ida.
	// and reclass looking.
	// yea, same rtti.

	// using this func, we actually got our own head position of the local player.
	// it's not always the first one, but it's at the top.
	// check by address like here. if the entity is actually the local player.
	// camera on is your local player.
	// so how we make aimbot from this?
	// yea
	// yea
	// yeyeye
	// the pitch we worry about later.
	// let's write a aimbot for only yaw now.

	// we dont care about target selection here, we just pick the [1] target in the enitty list
	// let's actually exclude the local player from being put in the enitty list.

	//return Coms->Read<Vector3>(VisualState + 0x168);
	//return Coms->Read<Vector3>(Camera + 0x2c);
	HeadPos2 = Coms->ReadVirtual<Vector3>(Camera + 0x2c);
}

void Entity::CacheHeadPosition(bool State) {

	// You can read Vector3 at once. i was just stupid in the manware source lol
	// 0x168 for head right?
	// or is it chest?
	// what was it.
	// 0x198 is gun angles
	// iirc
	//auto test = Coms->Read<Vector3>(VisualState + 0x168);
	HeadPos = Coms->ReadVirtual<Vector3>(VisualState + 0x168);    // i think this is either head or chest, idk.
}




bool Entity::GetDead() {
	return dead;
}

Vector3 Entity::GetFeetPosition() const{
	return FeetPos;
}

Vector3 Entity::GetHeadPosition() const{
	return HeadPos;
}

// I'll make a quick thing to get entities.

std::string Entity::GetType(bool State) {
		const auto EntityType = Coms->ReadVirtual<UINT64>(m_Base + 0x150);

		const auto StringEntry = Coms->ReadVirtual<UINT64>(EntityType + 0xD0);    // i think correct

		return Coms->ReadString(StringEntry + 0x10);
}

Vector3 Entity::FeetPosition() {
	VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x190);
	return FeetPos = Coms->ReadVirtual<Vector3>(VisualState + 0x2c);

}

Vector3 Entity::HeadPosition2(UINT64 ModuleBase) {	// dont use this. only for local player
	//const auto VisualState = Coms->Read(Address + 0xD0);

	const auto World = g_Client->m_World.GetBase();
	//Coms->Read<UINT64>(ModuleBase + 0x2674500);	// camera is only at local player.
	const auto Camera = Coms->ReadVirtual<UINT64>(World + 0xc70);

	// this visual state is "laggier" for other entities. so use the other one. +D0
	// but for local player, it's the only one that works + its fast on local player
	// trial and error + ida.
	// and reclass looking.
	// yea, same rtti.

	// using this func, we actually got our own head position of the local player.
	// it's not always the first one, but it's at the top.
	// check by address like here. if the entity is actually the local player.
	// camera on is your local player.
	// so how we make aimbot from this?
	// yea
	// yea
	// yeyeye
	// the pitch we worry about later.
	// let's write a aimbot for only yaw now.

	// we dont care about target selection here, we just pick the [1] target in the enitty list
	// let's actually exclude the local player from being put in the enitty list.

	//return Coms->Read<Vector3>(VisualState + 0x168);
	//return Coms->Read<Vector3>(Camera + 0x2c);
	return HeadPos = Coms->ReadVirtual<Vector3>(Camera + 0x2c);
}

Vector3 Entity::GunAngles() {
	const auto m_VisualState2 = Coms->ReadVirtual<UINT64>(m_Base + 0xD0);

	return Coms->ReadVirtual<Vector3>(m_VisualState2 + 0x180);
}

Vector3 Entity::HeadPosition() {
	const auto VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x190);

	// You can read Vector3 at once. i was just stupid in the manware source lol
	// 0x168 for head right?
	// or is it chest?
	// what was it.
	// 0x198 is gun angles
	// iirc
	//auto test = Coms->Read<Vector3>(VisualState + 0x168);
	return Coms->ReadVirtual<Vector3>(VisualState + 0x168);    // i think this is either head or chest, idk.
}

void Entity::WriteViewAngles(Vector3 Angles) {

	const auto VisualState1 = Coms->ReadVirtual<UINT64>(m_Base + 0xD0); //write to this visual state not the one in class.

	const auto RealPlayer = Coms->ReadVirtual<UINT64>(m_Base + 0x1118);    // I think the offset is 0x1080

	const auto PitchHeap = Coms->ReadVirtual<UINT64>(RealPlayer + 0x1A0); // find what this is

	// If you look in the visual state structure, there's a lot of view matrixes.
	// some of them are inverse.
	// if you write viewangles only on the 0x20 and 0x28, the weapon kinda rotates, and goes out of place.
	// it causes the aimbot to jitter. if you write all the view matrixes, it doesnt jitter.
	Coms->WriteVirtual<float>(VisualState1 + 0x20, Angles.x);
	Coms->WriteVirtual<float>(VisualState1 + 0x10, -Angles.x);
	Coms->WriteVirtual<float>(VisualState1 + 0x9C, Angles.x);
	Coms->WriteVirtual<float>(VisualState1 + 0xAC, -Angles.x);


	Coms->WriteVirtual<float>(PitchHeap + 0x4, Angles.y);


	Coms->WriteVirtual<float>(VisualState1 + 0x28, Angles.z);
	Coms->WriteVirtual<float>(VisualState1 + 0x08, Angles.z);
	Coms->WriteVirtual<float>(VisualState1 + 0x94, Angles.z);
	Coms->WriteVirtual<float>(VisualState1 + 0xB4, Angles.z);
}

void Entity::CacheVehVisualState(bool state) {
	m_VisualState2 = Coms->ReadVirtual<UINT64>(m_Base + 0xD0);

	HeadPos = Coms->ReadVirtual<Vector3>(m_VisualState2 + 0x2c);

	//HeadPos.y += 0.62f;

}

////////////////////////////////////////////////////////////////

void Vehicle::GetVehicleTransform(Vector3& Aside, Vector3& Up, Vector3& Front, Vector3& Position) {

	auto VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x180);

	Aside = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 0);
	Up = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 1);
	Front = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 2);
	Position = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 3);

	return;
}

/* returns null if there's no target in here, REMEMBER TO DO NULL PTR CHECK OR CRASH ! */
Entity* Vehicle::GetTargetInVehicle(Vector3& HeadPosition) {

	// What type of vehicle am i?

	if (m_Driver.m_Base == 0)
		return NULL;

	/* only works for driver.*/
	if (g_HeadLookup.find(CarName) != g_HeadLookup.end()) {
		auto ModelPos = g_HeadLookup.at(CarName);
		
		// resolve HeadPos
		// - do matrix multiplication with the visualstate transform
		auto VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x180);

		if (!VisualState)
			return NULL;

		float* Transform = new float[12];
		
		*(Vector3*)&Transform[0] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 0);
		*(Vector3*)&Transform[3] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 1);
		*(Vector3*)&Transform[6] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 2);
		*(Vector3*)&Transform[9] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 3);
		
		float _x = ModelPos.x * Transform[0] + ModelPos.y * Transform[3] + ModelPos.z * Transform[6] + Transform[9];
		float _y = ModelPos.x * Transform[1] + ModelPos.y * Transform[4] + ModelPos.z * Transform[7] + Transform[10];
		float _z = ModelPos.x * Transform[2] + ModelPos.y * Transform[5] + ModelPos.z * Transform[8] + Transform[11];

		HeadPosition = Vector3(_x, _y, _z);

		return &m_Driver; 
	}

	return &m_Driver;
}

/*
void aimbot() {
	Vector3 HeadBla;

	auto Target = Vehicle().GetTargetInVehicle(HeadBla);

	if (Target->GetDead())
		return false;
}
*/

void Vehicle::CacheVehicleVisualState(bool state) {
	VVisualState = Coms->ReadVirtual<UINT64>(m_Base + 0xD0);
	HHeadPos = Coms->ReadVirtual<Vector3>(VVisualState + 0x2c);
}

void Vehicle::CacheCleanName(bool State) {

	if (State) {

		auto EntityType = Coms->ReadVirtual<UINT64>(m_Base + 0x150);

		if (!EntityType)
			return;

		auto CleanNameEntry = Coms->ReadVirtual<UINT64>(EntityType + 0x13C8);

		auto CleanNameSize = Coms->ReadVirtual<INT32>(CleanNameEntry + 0x8);

		CarName = Coms->ReadString(CleanNameEntry+0x10);
	}
}

void Vehicle::CacheDriver(bool State) {

	if (State) {

		auto Driver = Coms->ReadVirtual<UINT64>(m_Base + 0xD70);

		if (Driver == 0x0) {
			HasDriver = false;
			return;
		}

		m_Driver.m_Base = Driver;

	}

	m_Driver.Cache(State);
	m_Driver.CacheVehVisualState(State);
}


void Vehicle::Cache(bool State) {

	if (!m_Base)
		return;

	CacheVehicleVisualState(State);
	CacheCleanName(State);
	CacheDriver(State);
}

void LocalPlayer::CacheLocal(bool State) {
	Cache(State); //it doesnt cache the whole
	CacheWeapon(State);
	CacheGunAngles(State);
}

void LocalPlayer::CacheWeapon(bool State) {
	if (State) {
		m_weapon.m_Base = m_Base + 0xC40;
		m_weapon.Cache(State);
	}
}


void LocalPlayer::CacheGunAngles(bool State) {
	
	auto HistoryVisualState = Coms->ReadVirtual<UINT64>(m_Base + 0xD0);
	GGunAngles = Coms->ReadVirtual<Vector3>(HistoryVisualState + 0x198);
	
}