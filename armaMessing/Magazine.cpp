#include "framework.h"

void Magazine::Cache(bool State) {

	CacheAmmo(State);

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