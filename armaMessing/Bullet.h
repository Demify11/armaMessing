#pragma once

struct VisualStateBullet
{
	uint8_t pad[0x28];
	Vector3 Pos;
	uint8_t pad1[0x50 - 0x34];
	Vector3 Velocity;


};

class Bullet
{

public:
	UINT64 m_Base;
	int32_t Timer;
private:
	UINT64 m_VisualStateBase;
	VisualStateBullet m_VisualState;
	Vector3 m_Position;
	Vector3 m_Velocity;
	UINT64 m_Owner;

public:
	void Cache(bool SlowCache);
	void CacheOwnerInit();
	

private:
	void CacheVisualState(bool SlowCache);
	void CacheBulletTimer();

public:
	Vector3 GetPosition();
	Vector3 GetVelocity();

};