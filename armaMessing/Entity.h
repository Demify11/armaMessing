
#pragma once
#include "mem.h"
#include "cmath"

// Can you read this?
// We make an entity class
struct Vector3 {
	float x, y, z;
	Vector3()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3 operator*(Vector3 vec)
	{
		return {
			x * vec.x,
			y * vec.y,
			z * vec.z
		};
	}

	Vector3 operator*(float vec)
	{
		return {
			x * vec,
			y * vec,
			z * vec
		};
	}

	Vector3 operator-(Vector3 vec)
	{
		return {
			x - vec.x,
			y - vec.y,
			z - vec.z
		};
	}

	Vector3 operator+(Vector3 vec)
	{
		return {
			x + vec.x,
			y + vec.y,
			z + vec.z
		};
	}

	Vector3 operator+(float vec)
	{
		return {
			x + vec,
			y + vec,
			z + vec
		};
	}

	Vector3 operator/(Vector3 vec)
	{
		return {
			x / vec.x,
			y / vec.y,
			z / vec.z
		};
	}

	Vector3 operator/(float vec)
	{
		return {
			x / vec,
			y / vec,
			z / vec
		};
	}

	bool operator==(Vector3 vec)
	{
		return (
			x == vec.x &&
			y == vec.y &&
			z == vec.z
		);
	}

	bool operator==(float vec)
	{
		return (
			x == vec &&
			y == vec &&
			z == vec
		);
	}

	bool operator !=(Vector3 vec) {
		return (
			x != vec.x && 
			y != vec.y && 
			z != vec.z  
		);
	}

	bool IsZero() {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	float Dot(Vector3 vec)
	{
		return ((x * vec.x) + (y * vec.y) + (z * vec.z));
	}

	float Distance(Vector3 vec)
	{
		return sqrtf((
			powf(x - vec.x, 2) +
			powf(y - vec.y, 2) +
			powf(z - vec.z, 2)
			));
	}

	bool Zero()
	{
		return (x == 0.f || y == 0.f || z == 0.f);
	}

	void Normalize()
	{
		float magnitude = sqrtf(this->Dot(*this));

		*this = *this / magnitude;
	}

	void print()
	{
		printf("%f | %f | %f\n", x, y, z);
	}

	float size()
	{
		return sqrtf((
			powf(x, 2) +
			powf(y, 2) +
			powf(z, 2)
			));
	}

};

struct Vector2 {
	float x, t;
};

class Entity {
private:
	/*
	UINT64 Address = 0; make variables private and make getters and setters.
	bool dead;
	Vector3 HeadPos;
	Vector3 HeadPos2;
	Vector3 FeetPos;
	UINT64 VisualState = 0;*/

	void CacheVisualState(bool State);
	void CacheDead(bool State);
	void CacheFeetPosition(bool State);
	void CacheHeadPosition2(bool State);
	void CacheHeadPosition(bool State);	
	

	// To know where the entity is placed in arma 3 memory. We hold the address in a value
public:
	UINT64 m_Base = 0;
	bool dead;
	Vector3 HeadPos;
	Vector3 HeadPos2;
	Vector3 FeetPos;
	UINT64 VisualState = 0;
	UINT64 m_VisualState2;
	Vector3 VehicleHeadPos;

	bool GetDead();

	// I'll make a quick thing to get entities.

	void Cache(bool State);
	std::string GetType(bool State);
	Vector3 FeetPosition();
	Vector3 HeadPosition2(UINT64 ModuleBase);
	Vector3 GunAngles();
	Vector3 HeadPosition();
	void WriteViewAngles(Vector3 Angles);
	Vector3 GetFeetPosition() const;
	Vector3 GetHeadPosition() const;

	void CacheVehVisualState(bool State);
};

class Local : public Entity {
private:
	UINT64 HistoryVisualState = 0x0; // 0xD0

};

static std::unordered_map<std::string, const Vector3> g_HeadLookup = {
	{"Hatchback (Sport)", { -0.373535,0.00910616,-0.142124 }},
	{"Ifrit", { 0.065918,-0.151881, -0.780082 }}
};

class Vehicle : public Entity {
public:
	int ga = 1; // haram
	UINT64 D0VisualState = 0;
	UINT64 VVisualState = 0;
	Vector3 HHeadPos;

	std::string CarName = "";

	bool HasDriver = false;

	Entity m_Driver;	// null if there's none!!!!
	std::vector<Entity> m_Passengers;	// same heree ^^

public:

	void GetVehicleTransform(Vector3& Aside, Vector3& Up, Vector3& Front, Vector3& Position);
	Entity* GetTargetInVehicle(Vector3& HeadPosition); // use this for aimbot selection
		
private:
	void CacheVehicleVisualState(bool State);

private:
	void CacheCleanName(bool State);
	void CacheDriver(bool State);

public:
	void Cache(bool State);
};

class LocalPlayer : public Entity {


public:
	Vector3 GGunAngles;
	Weapon m_weapon;

public:

	void CacheLocal(bool State);

private:

	void CacheWeapon(bool State);
	void CacheGunAngles(bool State);

};