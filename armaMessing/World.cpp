#include "framework.h"

bool Camera::WorldToScreen(Vector3 World, Vector3& Screen) {

	auto Relative = Vector3{
	World.x - CachedViewPosition.x,
	World.y - CachedViewPosition.y,
	World.z - CachedViewPosition.z,
	};

	float _x = Relative.Dot(CachedViewRight);
	float _y = Relative.Dot(CachedViewUp);
	float _z = Relative.Dot(CachedViewAside);

	Screen = Vector3{
		CachedViewPort.x * (1 + (_x / CachedViewProjD1.x / _z)),
		CachedViewPort.y * (1 - (_y / CachedViewProjD2.y / _z)),
		_z
	};

	return _z >= 1.5f;
}

void Camera::Init(UINT64 _Base) {

	Base = _Base;

}

void Camera::Cache(bool State) {

	if (State) {
		// Slow cache ran every 500 ms.


	}

	//struct QuickContainer {
	//	Vector3 a1;			// 0x0	- 0x8
	//	Vector3 a2;			// 0xC	- 0x14
	//	Vector3 a3;			// 0x
	//	Vector3 a4;			// MOREC ODE ON SCREEN MORE PRODUCTIVITY :)
	//	int a5_undef; all my friends hate scrollbars, so cringe :(
	//	UINT64 a6_undef;
	//	UINT64 a7_undef;
	//	// but its bad to manually make, which is why trappo had the DEFINE_MEMBER_N but it just doesnt allow for auto-updates
	//	// well it could, but you'd have to make a bit more advanced system.
	//	// you could also do smth like this.
	//};

	struct PageRead {
		char Page_0000[0xDC+sizeof(Vector3)];	// can also do like this.
		//apparently ¨ווווווווווווווווווווווווווווור'ר'ר¨וhahaha
		// do you want that stuff on?
	};

	// i have like this on my cheat
	// and this is fine
	// in theory you're only gonna read once every 16 ms, so aslong as cache is less than 16 ms

	// say again?
	// yea
	// a vector3 is 3xfloats
	// and a float is 4 bytes
	// so a char is 0x4 + 0x4 + 0x4 = 0xC
	// or 12 bytes in decimal

	auto data = Coms->ReadVirtual<PageRead>(Base + 0x00);	// this is also semi unsafe imo.
	// cause if this doesnt "read" correctly, uou might have invalid memory, and then this below
	// vv will cause an status_violation_access error or whatever it's called. Im not sure tho, but it seems risky.
	
	// yea, you'd be messed up. most likely.
	// 


	CachedViewRight		= *(Vector3*)(data.Page_0000 + 0x8);	// and info still seems intact, but you have this massive read
	CachedViewUp		= *(Vector3*)(data.Page_0000 + 0x14);
	CachedViewAside		= *(Vector3*)(data.Page_0000 + 0x20);
	CachedViewPosition	= *(Vector3*)(data.Page_0000 + 0x2C);
	CachedViewPort		= *(Vector3*)(data.Page_0000 + 0x58);
	CachedViewProjD1	= *(Vector3*)(data.Page_0000 + 0xD0);
	CachedViewProjD2	= *(Vector3*)(data.Page_0000 + 0xDC);	// last offset. so we read from 0x00 to 0xDC + 0xC
	// yea


	//CachedViewRight = Coms->Read<Vector3>(Base + 0x8);
	//CachedViewUp = Coms->Read<Vector3>(Base + 0x14);
	//CachedViewAside = Coms->Read<Vector3>(Base + 0x20);
	//CachedViewPosition = Coms->Read<Vector3>(Base + 0x2C);
	//CachedViewPort = Coms->Read<Vector3>(Base + 0x58);
	//CachedViewProjD1 = Coms->Read<Vector3>(Base + 0xD0);
	//CachedViewProjD2 = Coms->Read<Vector3>(Base + 0xDC);
}

void World::Init(UINT64 _Base) {

	Base = _Base;

}

void World::CacheCamera(bool State) {

	if (State) {
		// Slow cache ran every 500 ms.

		// Get Camera Base Address and apply. vv Checking ptr for camera.

		auto CameraBase = Coms->ReadVirtual<UINT64>(Base + 0xD30);

		m_Camera.Init(CameraBase);

	}

	// Cache ran every 16 ms. -> Every 500 ms true is passed in to let other classes have a chance of checking ptrs.

	m_Camera.Cache(State);

}

void World::CacheLocalPlayer(bool State) {
	if (State) {
		auto buff = Coms->ReadVirtual<UINT64>(Base + 0x2C38);

		m_LocalPlayer.m_Base = Coms->ReadVirtual<UINT64>(buff + 0x8);
	}
	m_LocalPlayer.CacheLocal(State);

}

void World::CacheEntityList(bool State) {
	/*
	auto TempList = std::vector<Entity>();
	auto VehicleList = std::vector<Vehicle>();

	if (State) {
		PushbackEntity(VehicleList, TempList, g_Client->m_World.Base, 0x1C28, g_Client->m_World.localPlayer);// We have to do + 8, cause at offset 0x0 is the class vtable.
		PushbackEntity(VehicleList, TempList, g_Client->m_World.Base, 0x1CF0, g_Client->m_World.localPlayer);
		PushbackEntity(VehicleList, TempList, g_Client->m_World.Base, 0x1db8, g_Client->m_World.localPlayer);
		PushbackEntity(VehicleList, TempList, g_Client->m_World.Base, 0x1E80, g_Client->m_World.localPlayer);
	} else {
		TempList = Entities;
		VehicleList = Vehicles;
	}

	// Before you had it so it would constantly add entities to the same list, without clearing up.
	// resulting in ALOT of duplicates.

	for (auto& Entry : TempList) {
		Entry.Cache(true);
	}

	for (auto& Entry : VehicleList) {
		Entry.Cache(State);
	}

	Entities = TempList;
	Vehicles = VehicleList;
	*/

	std::unordered_map<uintptr_t, Entity> tempEntities = entityCache;
	std::unordered_map<uintptr_t, Vehicle> tempVehicles = vehicleCache;


	if (State) {
		// Temporary collections for this update tick
		std::unordered_map<uintptr_t, Entity> newEntities;
		std::unordered_map<uintptr_t, Vehicle> newVehicles;

		// Populate temporary data from game memory
		for (uint32_t offset : { 0x1C28, 0x1CF0, 0x1DB8  }) {//0x1E80
			GatherEntitiesAndVehiclesAtOffset(
				g_Client->m_World.Base,
				offset,
				g_Client->m_World.localPlayer,
				newEntities,
				newVehicles
			);
		}

		// Apply delta update (add new, update changed, remove old)
		UpdateMapWithDelta(tempEntities, newEntities);
		UpdateMapWithDelta(tempVehicles, newVehicles);
	}

	// Tick-update entity state every frame
	for (auto& [_, entity] : tempEntities) {
		entity.Cache(true);
	}
	/*
	for (auto& [_, vehicle] : tempVehicles) {
		vehicle.Cache(State);
	}*/

	// this is not atomic
	entityCache = tempEntities; 
	tempVehicles = tempVehicles;
}

void World::Cache(bool State) {

	CacheCamera(State);
	CacheLocalPlayer(State);
	CacheEntityList(State);

}

void Client::Cache(bool State) {
	
	if (State) {
		// Slow cache ran every 500 ms.
		// used to check ptrs.

		auto WorldBase = Coms->ReadVirtual<UINT64>(ModuleBase + Offsets::ModBase::World);

		m_World.Init(WorldBase);
	}

	m_World.Cache(State);
}

bool World::IsInFOV(Vector3 pos) {
	Vector3 centre;
	centre.x = m_Camera.CachedViewPort.x;
	centre.y = m_Camera.CachedViewPort.y;

	float distance = powf((pos.x - centre.x), 2) + powf((pos.y - centre.y), 2);
	float radSqr = pow(400, 2);

	if (distance < radSqr) {
		return true;
	}
	else {
		return false;
	}

}