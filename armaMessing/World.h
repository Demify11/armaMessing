#pragma once

/*
caching, is less reads
less reads is usually better performance

driver and syscalls are very expensive
so if you can minimize or "reuse" a read, by caching it, it's defo worth to cache. 

if you get confused look at the camera as that shows how i like to do my cache.

----

The reason why i have a "bool State"

is because I seperate my caching into two types of cache

A fast cache and a slow cache

The fast cache reads stuff that needs to be read fast
Positions, is dead, lifestate, invis stuff like that

My slow cache reads stuff that doesnt need to be updated that fast
Pointer reads, such as visualstates base addr. or in this case of the camera, the camera bases address.

or the world as in Client::Cache

the first four are, so you could in theory do smth like this.


*/

/*
The battle plan

- Make an easy way to get entities from an entitylist.
- Cache Entities into world.
- Make an Entity Class
- Have the Entity class being able to cache it's value
- Have a VisualState class that has the ability to cache it's value
- Make a getter for VisualState in Entity. (Look at GetWorld() in Client to get an idea)

*/

class Camera {
public:
	UINT64 Base = 0;

	Vector3 CachedViewRight	    = {0,0,0};
	Vector3 CachedViewUp	    = {0,0,0};
	Vector3 CachedViewAside	    = {0,0,0};
	Vector3 CachedViewPosition  = {0,0,0};
	Vector3 CachedViewPort	    = {0,0,0};
	Vector3 CachedViewProjD1    = {0,0,0};
	Vector3 CachedViewProjD2    = {0,0,0};

	bool WorldToScreen(Vector3 World, Vector3& Screen);

	void Init(UINT64 _Base);
	void Cache(bool State);
};

class World {
private:
	UINT64 Base = 0;
	Camera m_Camera;
	std::vector<Entity>Entities;
	std::vector<Vehicle>Vehicles;
public:
	std::unordered_map<uintptr_t, Entity> entityCache;
	std::unordered_map<uintptr_t, Vehicle> vehicleCache;
private:

	UINT64 localPlayer = 0;
	

public:
	LocalPlayer m_LocalPlayer;
	auto GetCamera() -> Camera* { return &m_Camera; } //in next project use getters to get back member variables
	auto GetEntities() { return Entities; }
	auto GetVehicles() { return Vehicles; }
	auto GetBase() -> UINT64 {
		return Base;
	}

private:

	void CacheCamera(bool State);
	void CacheEntityList(bool State);
	void CacheLocalPlayer(bool State);

public:

	void Init(UINT64 _Base);
	void Cache(bool State);

	bool IsInFOV(Vector3 pos);
};

class Client {
public:
	UINT64 ModuleBase = 0;

	World m_World;

	auto GetWorld() -> World* { return &m_World; }

	void Cache(bool State);
};

extern Client* g_Client;