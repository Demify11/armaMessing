#include "framework.h"

void Entity::Cache(bool State) {	// FOR EACH ENTITY YOU DO 3 READS - 100 * 3

	//auto Pre = Coms->GetReads();

	CacheVisualState(State);	// 2 read
//	CacheHeadPosition(State);	// 0 read
//	CacheFeetPosition (State);	// 0 read
//	CacheHeadPosition2 (State);o cached if not local player!!!
	CacheDead(State);			// 1 read

	//auto Post = Coms->GetReads();

	//printf("[DEBUG] \t-ENTITY#CACHE: %i \n", Post - Pre);

}

void Entity::CacheDead(bool State) {
	dead = !(Coms->ReadVirtual<UINT8>(m_Base + 0x5CC) & 1);
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

	VisualCamera = *(PageVRead*)(Data.pad_0000 + 0x8);
	m_FeetPos = *(Vector3*)(Data.pad_0000 + 0x2C);
	m_HeadPos = *(Vector3*)(Data.pad_0000 + 0x168);
	m_Velocity = *(Vector3*)(Data.pad_0000 + 0x54);
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


bool Entity::GetDead() {
	return dead;
}

void Entity::InitNetworkId()
{
	m_NetworkId = Coms->ReadVirtual<int>(m_Base + 0xC34);

	if (m_NetworkId != -1 || HasName == false) {

	
		auto it = g_Client->m_NetworkManager.Identities.find(m_NetworkId);

		if (it != g_Client->m_NetworkManager.Identities.end()) {
			m_Name = it->second.m_Name;
		}
	}
	HasName = true;
}

Vector3 Entity::GetFeetPosition() const{
	return m_FeetPos;
}

Vector3 Entity::GetHeadPosition() const{
	return m_HeadPos;
}

// I'll make a quick thing to get entities.

std::string Entity::GetType(bool State) {
		const auto EntityType = Coms->ReadVirtual<UINT64>(m_Base + 0x150);

		const auto StringEntry = Coms->ReadVirtual<UINT64>(EntityType + 0xD0);    // i think correct

		return Coms->ReadString(StringEntry + 0x10);
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
	return m_HeadPos = Coms->ReadVirtual<Vector3>(Camera + 0x2c);
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

std::string Entity::ReadCategory(UINT64 base) {

	const auto catBuf = Coms->ReadVirtual<uint64_t>(base + 0x150);
	if (!catBuf) return {};

	const auto catPtr = Coms->ReadVirtual<uint64_t>(catBuf + 0xD0);
	if (!catPtr) return {};

	const std::string category = Coms->ReadString(catPtr + 0x10);

	return category;
}

Entity* Entity::Create(UINT64 base) {
	const std::string category = ReadCategory(base);

	Entity* e;
	EntityType t;

	if (category == "carx") {
		e = new Vehicle;   
		t = EntityType::Vehicle;
	}
	else if (category == "soldier") {
		e = new Entity();
		t = EntityType::Player;
	}
	else {
		e = new Entity();
		t = EntityType::Junk;
	}

	e->m_Base = base;
	e->type = t;
	e->InitNetworkId();
	e->OnClassify();      // virtual hook for setup
	e->classified = true;
	return e;

}

////////////////////////////////////////////////////////////////


/* returns null if there's no target in here, REMEMBER TO DO NULL PTR CHECK OR CRASH ! */


/*
void aimbot() {
	Vector3 HeadBla;

	auto Target = Vehicle().GetTargetInVehicle(HeadBla);

	if (Target->GetDead())
		return false;
}
*/
/*
void Entity::CacheVehicleVisualState(bool state) {

	if (state) {

		vehicle->VehVisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x180);
		vehicle->VehHeadPos = Coms->ReadVirtual<Vector3>(vehicle->VehVisualState + 0x2c);

	}
	vehicle->VehVelocity = Coms->ReadVirtual<Vector3>(vehicle->VehVisualState + 0x54);
	vehicle->VehVisualPage = Coms->ReadVirtual<VehicleData::PageRead>(vehicle->VehVisualState + 0x8);
}*/


Vector3 Entity::GetHeadPos() {
	return m_HeadPos;
}

Vector3 Entity::GetVelocity() {
	return m_Velocity;
}

float LocalPlayer::GetInitSpeed() {

	
	return 0.0;
}

void LocalPlayer::CacheLocal(bool State) {
	Cache(State); //it doesnt cache the whole
	//CacheWeapon(State);
	CacheGunAngles(State);
}

void LocalPlayer::CacheWeapon(bool State) {
	if (State) {
		m_weapon.m_Base = m_Base; // + 0xC40; is WeaponsStatePerson. I dont really know what this is.
		m_weapon.Cache(State);
	}
}


void LocalPlayer::CacheGunAngles(bool State) {
	
	auto HistoryVisualState = Coms->ReadVirtual<UINT64>(m_Base + 0xD0);
	GGunAngles = Coms->ReadVirtual<Vector3>(HistoryVisualState + 0x198);
	
}