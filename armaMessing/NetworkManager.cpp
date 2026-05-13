#include "framework.h"

void NetworkManager::Cache(bool State) {

	CacheNetworkClient(State);
	CacheIdentities(State);

}

void NetworkManager::CacheNetworkClient(bool State) {

	if (State) {
		m_Identities = Coms->ReadVirtual<UINT64>(m_Base + Offsets::NetworkClient); //290
	}

}

void NetworkManager::CacheIdentities(bool State) {

	if (State) {

		auto Array = AutoArray(m_Identities + Offsets::PlayerIdentities);

		auto ArraySize = Array.GetSize();

		if (!Array.Allocate(ArraySize, 0x0, 0x290));
			return;

		for (int i = 0; i < ArraySize; i++) {

			Identity Temp;
			Temp.m_Base = Array.Get(i);
			Temp.Cache(State);

			Identities[Temp.m_NetworkId] = Temp;

		}

	}

}