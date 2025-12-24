#pragma once

template<typename T = UINT64>	// we cannot have anything in cpp with this..
class AutoArray {
private:
	UINT64 m_Base = 0;
	UINT32 m_Size = 0;		// 0x8 size!
	UINT32 m_Allocated = 0;	// 0xC size!

	// allocated stuff
	PBYTE m_Contents;
	UINT32 m_Start;
	UINT32 m_Offset;
	int m_AllocSize;

public:
	AutoArray(UINT64 Base) {
		(void)Init(Base);
	}

	~AutoArray() {
		(void)Release();
	}

public:
	bool Allocate(UINT32 Size, UINT32 Start = 0x0, UINT32 Offset = 0x8) {

		m_Start = Start;
		m_Offset = Offset;

		m_AllocSize = Start + (Offset * Size);

		m_Contents = new BYTE[m_AllocSize];

		if (!Coms->ReadVirtualBuffer(Coms->ReadVirtual<UINT64>(m_Base), m_Contents, m_AllocSize))
			return false;

		return true;
	}

	T Get(int Index, UINT32 OverwriteOffset = 0) {
		return *(T*)(m_Contents + m_Start + (m_Offset * Index) + OverwriteOffset);
	}

public:
	UINT32 GetSize(UINT32 Set = 0x8) {
		if (!m_Size)
			CacheSizes(true, Set);

		return m_Size;
	}

	UINT32 GetAllocated() {
		if (!m_Allocated)
			CacheSizes(true);

		return m_Allocated;
	}

private:
	void CacheSizes(bool State, UINT32 Set = 0x8) {

		if (State) {

			const auto Both = Coms->ReadVirtual<UINT64>(m_Base + Set);

			m_Size = *(UINT32*)((UINT64)(&Both) + 0x0);
			m_Allocated = *(UINT32*)((UINT64)(&Both) + 0x4);

		}
	}

public:
	void Cache(bool State) {

		CacheSizes(State);

	}

public:
	bool Init(UINT64 Base) {

		m_Base = Base;

		return m_Base;	// return true if valid!
	}

	bool Release() {
		delete[] m_Contents;

		return true;
	}
};