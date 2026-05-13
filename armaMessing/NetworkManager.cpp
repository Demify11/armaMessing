#include "framework.h"

void NetworkManager::Cache(bool State) {

	CacheNetworkClient(State);
	CacheIdentities(State);

}

void NetworkManager::CacheNetworkClient(bool State) {

	if (State) {
		m_NetworkClient = Coms->ReadVirtual<UINT64>(m_Base + Offsets::NetworkClient); //290
	}

}

struct IdentityEntry
{
	uint8_t  pad0[0x8];
	int32_t  m_NetworkId;

	uint8_t  pad1[0x150 - 0xC];
	uint64_t m_NamePtr;

	uint8_t  pad2[0x290 - 0x158];
};

static_assert(offsetof(IdentityEntry, m_NetworkId) == 0x8);
static_assert(offsetof(IdentityEntry, m_NamePtr) == 0x150);
static_assert(sizeof(IdentityEntry) == 0x290);

void NetworkManager::CacheIdentities(bool State) { //we cant really use the autoarray class becuase the class assumes data is located
	// at the addresses of each index, however this list is in stride/inline. need to update autoarray for this senario

	if (State) {

		auto Entry = Coms->ReadVirtual<UINT64>(m_NetworkClient + Offsets::PlayerIdentities);
		auto ListSize = Coms->ReadVirtual<int>(m_NetworkClient + Offsets::PlayerIdentities + 0x8);

		//uint8_t* Buffer = new uint8_t[ListSize * 0x290];

		IdentityEntry* Entries = new IdentityEntry[ListSize];

		//memset(Entries, 0x00, ListSize * 0x290);

		if (!Coms->ReadVirtualBuffer(Entry, Entries, ListSize * 0x290)) {
			delete[] Entries;
			return;
		}

		//auto* entries = reinterpret_cast<IdentityEntry*>(Buffer); dont need this anymore becuase we directly make a buffer of entries

		for (int i = 0; i < ListSize; i++)
		{
			IdentityEntry& entry = Entries[i];

			auto& cached = Identities[entry.m_NetworkId];

			cached.m_NetworkId = entry.m_NetworkId;

			if (entry.m_NamePtr)
			{
				if (cached.m_NamePtr != entry.m_NamePtr)
				{
					cached.m_NamePtr = entry.m_NamePtr;
					cached.m_Name = Coms->ReadString(entry.m_NamePtr + 0x10);
				}
			}
		}
		
		delete[] Entries;

	}
	//Need a way to remove dead entities.
}