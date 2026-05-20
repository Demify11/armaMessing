#include "framework.h"

void Magazine::Cache(bool State) {

	CacheAmmo(State);
	CacheMagType(State);

}

void Magazine::Init(UINT64 base) {
	m_Base = base;
}

void Magazine::CacheAmmo(bool State) {

	uint32_t key = 0xEE0B9D2E;

	uint32_t part1 = Coms->ReadVirtual<uint32_t>(m_Base + 0x8C);

	uint32_t part2 = Coms->ReadVirtual<uint32_t>(m_Base + 0x94);

	Decrypt(key, part1, part2);

}

uint32_t Magazine::Decrypt(uint32_t key, uint32_t part1, uint32_t part2) {

	auto val = (part1 + part2) ^ key;

	return val;

}

void Magazine::CacheMagType(bool State) {

	if (State) {
		auto MagType = Coms->ReadVirtual<UINT64>(m_Base + 0x48);
		auto AmmoType = Coms->ReadVirtual<UINT64>(MagType + 0x28);

		m_Gravity = Coms->ReadVirtual<float>(AmmoType + 0x408);
		m_AirFriction = Coms->ReadVirtual<float>(AmmoType + 0x400);
	}

}