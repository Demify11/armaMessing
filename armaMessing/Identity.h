#pragma once
class Identity
{
public:
	UINT64 m_Base;
	int m_NetworkId;
	std::string m_Name;

	void Cache(bool State);

private:



	void CacheIdentInfo(bool State);
};

