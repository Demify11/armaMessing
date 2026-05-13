#pragma once
class Identity
{
public:
	UINT64 m_Base;
	int m_NetworkId;

	void Cache(bool State);

private:

	std::string m_Name;

	void CacheIdentInfo(bool State);
};

