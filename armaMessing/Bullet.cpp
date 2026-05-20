#include "framework.h"

void Bullet::Cache(bool SlowCache) {
	CacheVisualState(SlowCache);
	CacheBulletTimer();
}

void Bullet::CacheOwnerInit() {

	auto OwnerBuff = Coms->ReadVirtual<UINT64>(m_Base + 0x5A0);
	m_Owner = Coms->ReadVirtual<UINT64>(OwnerBuff + 0x8);

}


void Bullet::CacheVisualState(bool SlowCache) {

	m_VisualStateBase = Coms->ReadVirtual<UINT64>(Offsets::LocalVisualState);
	m_VisualState = Coms->ReadVirtual<VisualStateBullet>(m_VisualStateBase + 0xD0);

}

Vector3 Bullet::GetPosition() {
	return m_Position;
}

Vector3 Bullet::GetVelocity() {
	return m_Velocity;
}


void Bullet::CacheBulletTimer() {

}