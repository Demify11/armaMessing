#pragma once
class Magazine
{
private:
	UINT64 Ammoaddr1;
	UINT64 Ammoaddr2;
	UINT64 MagazineAddr;

	UINT64 m_Base;

	void Encrypt();
	uint32_t Decrypt(uint32_t key, uint32_t part1, uint32_t part2);

public:
	void Init(UINT64 base);
	void Cache(bool State);
	void CacheAmmo(bool State);

};

