/*
#pragma once
#include<vector>
#include<windows.h>
#include<Tlhelp32.h>
#include<iostream>

class MemInterface {
private:
	HANDLE m_Process;
	DWORD m_ProcessId;

public:
	MemInterface();
	~MemInterface();

private:
	DWORD GetProcessID(const wchar_t* ExecutableName);

public:
	UINT64 GetModuleBase(const wchar_t* ModuleName);
	std::string ReadString(UINT64 Address, UINT32 Size = 1024);

	template<typename T = UINT64>
	T Read(UINT64 Address) {
		T Buffer = {};

		ReadProcessMemory(m_Process, (LPCVOID)(Address), &Buffer, sizeof(T), nullptr);

		return Buffer;
	}

	template<typename T = UINT64>
	void Write(UINT64 Address,T newVal) {
		DWORD oldprotect;
		//VirtualProtectEx(m_Process, Address, newVal.size(), PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(m_Process, (LPVOID)Address, &newVal, sizeof(T), nullptr);
		//VirtualProtectEx(m_Process, Address, newVal.size() , oldprotect, &oldprotect);
	}

public:
	bool Init(const wchar_t* ExecutableName);
};

extern MemInterface* Coms;
*/