#pragma once
class Magazine
{
private:
	UINT64 Ammoaddr1;
	UINT64 Ammoaddr2;
	UINT64 MagazineAddr;

	void Encrypt();
	void Decrypt();

public:
	void Cache(bool State);
	void CacheAmmo(bool State);

};

