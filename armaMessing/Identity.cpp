#include "framework.h"

void Identity::Cache(bool State) {
	CacheIdentInfo(State);
}

void Identity::CacheIdentInfo(bool State) {

	auto Buffer = Coms->ReadVirtual<UINT64>(m_Base + 0x150);
	m_Name = Coms->ReadString(Buffer);

	m_NetworkId = Coms->ReadVirtual<int>(m_Base + 0x8);

}