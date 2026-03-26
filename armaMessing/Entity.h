
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
	/* blufor */
		{ "AH-9 Pawnee",{-0.302734,0.218747,1.23325} },
		{ "AH-99 Blackfoot",{0.03125,0.113494,3.83582} },
		{ "CH-67 Huron",{0.460938,-0.312252,6.4907} },
		{ "CH-67 Huron (Unarmed)",{0.458496,-0.313191,6.49192} },
		{ "MH-9 Hummingbird",{-0.302734,0.218869,1.23316} },
		{ "UH-80 Ghost Hawk",{-0.517578,-0.238793,5.14418} },
		{ "HEMTT",{-0.732422,0.484195,3.20634} },
		{ "HEMTT Ammo",{-0.755371,0.470943,4.00647} },
		{ "HEMTT Box",{-0.8125,0.0850735,4.17009} },
		{ "HEMTT Cargo",{-0.810547,0.0847096,4.86881} },
		{ "HEMTT Flatbed",{-0.733887,-0.305303,4.87718} },
		{ "HEMTT Fuel",{-0.730469,0.483295,3.82427} },
		{ "HEMTT Medical",{-0.916992,0.184648,4.07904} },
		{ "HEMTT Repair",{-0.805664,0.084034,4.17605} },
		{ "HEMTT Transport",{-0.78418,0.174357,4.03085} },
		{ "HEMTT Transport (Covered)",{-0.743652,0.171693,4.04595} },
		{ "Hunter",{-0.45752,0.226528,-1.41125} },
		{ "Hunter GMG",{-0.457031,-0.325842,-1.37607} },
		{ "Hunter HMG",{-0.491211,-0.326932,-1.39492} },
		{ "Prowler (AT)",{-0.652832,-0.143311,0.275825} },
		{ "Prowler (HMG)",{-0.682129,-0.457396,0.258488} },
		{ "Prowler (Unarmed)",{-0.617676,-0.45787,0.266443} },
		{ "Quad Bike",{0.0136719,0.278324,-0.0812217} },
		{ "Assault Boat",{0.38916,-0.282517,-1.28642} },
		{ "Rescue Boat",{0.430664,-0.282515,-1.30479} },
		{ "Speedboat Minigun",{0.195313,-0.357894,0.829717} },

		/* opfor */
		{"Mi-290 Taru", {-0.660645,-0.326407,4.00736}},
		{"Mi-290 Taru (Ammo)", {-0.660645,-0.0896692,4.00561}},
		{"Mi-290 Taru (Bench)", {-0.661133,-0.326394,4.00725}},
		{"Mi-290 Taru (Cargo)", {-0.660645,-0.0902448,4.00699}},
		{"Mi-290 Taru (Fuel)", {-0.661133,0.0557466,4.00789}},
		{"Mi-290 Taru (Medical)", {-0.660645,-0.0903482,4.00717}},
		{"Mi-290 Taru (Medical)", {-0.661133,-0.0900707,4.00659}},
		{"Mi-290 Taru (Repair)", {-0.661621,-0.0909109,4.00824}},
		{"Mi-290 Taru (Transport)", {-0.661621,-0.0909963,4.00848}},
		{"Mi-48 Kajman", {-0.0336914,0.3307,3.00349}},
		{"PO-30 Orca", {0.407715,-0.371259,3.37001}},
		{"PO-30 Orca (Unarmed)", {0.407715,-0.371264,3.36988}},
		{"Ifrit", {0.0239258,-0.00692272,-0.811817}},
		{"Ifrit GMG", {-0.027832,-0.356136,-0.810528}},
		{"Ifrit HMG", {-0.0424805,-0.354923,-0.821773}},
		{"Qilin (AT)", {-0.181152, -0.0466356, 0.534812}},
		{"Qilin (Minigun)", {-0.181152, -0.0466356, 0.534812}},
		{"Qilin (Unarmed)", {-0.181152, -0.0466356, 0.534812}},
		{"Tempest (Device)", {-0.737793,0.703078,2.25049}},
		{"Tempest Ammo", {-0.760254,0.704107,2.2344}},
		{"Tempest Fuel", {-0.745605,0.702507,2.32065}},
		{"Tempest Medical", {-0.634277,0.429929,2.5205}},
		{"Tempest Repair", {-0.700195,0.703941,2.23754}},
		{"Tempest Transport", {-0.671875,0.435176,2.52636}},
		{"Tempest Transport (Covered)", {-0.679688,0.435751,2.51474}},
		{"Zamak Ammo", {-0.639648,0.258454,2.89455}},
		{"Zamak Fuel", {-0.596191,0.258611,2.71802}},
		{"Zamak Medical", {-0.57373,0.258361,2.69842}},
		{"Zamak Repair", {-0.587891,0.258474,2.89691}},
		{"Zamak Transport", {-0.600098,0.258272,2.71579}},
		{"Zamak Transport (Covered)", {-0.612305,0.258296,2.70563}},
		{"Speedboat HMG", {0.0688477,1.71849,0.110171}},

		/* independant */
		{"CH-49 Mohawk",{0.748047,-1.10152,6.34361}},
		{"WY-55 Hellcat",{0.485352,0.320327,3.1578}},
		{"WY-55 Hellcat (Unarmed)",{0.484863,0.134992,3.15786}},
		{"Strider",{-0.0102539,0.132863,0.937112}},
		{"Strider GMG",{-0.0332031,-0.561211,0.923776}},
		{"Strider HMG",{-0.0253906,0.792559,-0.155429}},
		{"Zamak Ammo",{-0.587891,0.258534,2.89691}},
		{"Zamak Fuel",{-0.574707,0.258561,2.70008}},
		{"Zamak Medical",{-0.596191,0.258478,2.71813}},
		{"Zamak Repair",{-0.639648,0.258536,2.89451}},
		{"Zamak Transport",{-0.608887,0.258446,2.7094}},
		{"Zamak Transport (Covered)",{-0.568359,0.258439,2.71986}},

		/* civillian */
		{"M-900", {-0.309082,1.40305,2.04028}},
		{"Fuel Truck", {-0.506836, 0.473392, 0.0547505}},
		{"Hatchback", {-0.353516,-0.00777149,-0.0718043}},
		{"Hatchback (Sport)", {-0.355469,-0.00791693,-0.0681505}},
		{"Kart", {0.0522461, -0.0828342, -0.239975}},
		{"Kart (Bluking)", {0.0522461, -0.0828342, -0.239975}},
		{"Kart (Fuel)", {0.0522461, -0.0828342, -0.239975}},
		{"Kart (Redstone)", {0.0522461, -0.0828342, -0.239975}},
		{"Kart (Vrana)", {0.0522461, -0.0828342, -0.239975}},
		{"MB 4WD", {-0.366699,0.182532, -0.00230581}},
		{"Offroad", {-0.372559, 0.216817, 0.22662}},
		{"Offroad (Comms)", {-0.37207, 0.107836, 0.398185}},
		{"Offroad (Covered)", {-0.370605, 0.114599, 0.392483}},
		{"Offroad (Services)", {-0.372559, 0.216817, 0.22662}},
		{"SUV", {-0.384766,0.127804, -0.274392}},
		{"Tractor", {-0.0107422, 0.605633, -0.669401}},
		{"Truck", {-0.475586, 0.470151, 0.175949}},
		{"Truck Boxer", {-0.506836, 0.473392, 0.0547505}},
		{"Van (Ambulance)", {-0.489746, 0.192809, 2.16997}},
		{"Van (Cargo)", {-0.489746, 0.192809, 2.16997}},
		{"Van (Services)", {-0.489746, 0.192809, 2.16997}},
		{"Van Transport", {-0.489746, 0.192809, 2.16997}},
		{"Zamak Fuel", {-0.625488,0.257645,2.71889}},
		{"Zamak Repair", {-0.583008,0.25784,2.89282}},
		{"Zamak Transport", {-0.544434,0.258173,2.70477}},
		{"Zamak Transport (Covered)", {-0.571777,0.258175,2.72054}},
		{"RHIB", {-0.0761719, 0.985135, -1.27226}},
		{"Water Scooter", {0.00244141, 0.163588, 0.325972}},
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