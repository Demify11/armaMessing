#pragma once
class NetworkManager
{
public:
	UINT64 m_Base = 0;
	std::unordered_map<int, Identity> Identities;
	
	void Cache(bool State);

private:
	UINT64 m_NetworkClient;
	UINT64 m_Identities;
	std::string m_Name;
	int m_NetworkId;

	void CacheIdentities(bool State);
	void CacheNetworkClient(bool State);



};

