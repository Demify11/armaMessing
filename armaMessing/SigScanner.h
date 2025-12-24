#include "framework.h"

class SigScanner {

private: 
	UINT64 m_ModuleBase;
	DWORD m_ModuleSize;

	BYTE* m_TextBuffer;
	UINT32 m_TextBufferOffset;
public:
	DWORD GetArmaProcessID() {

		HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 Pe32;
		Pe32.dwSize = sizeof(Pe32);

		if (Process32First(Snapshot, &Pe32)) {
			do {

				if (!wcscmp(Pe32.szExeFile, L"arma3_x64.exe")) {

					CloseHandle(Snapshot);

					return Pe32.th32ProcessID;
				}

			} while (Process32Next(Snapshot, &Pe32));
		}

		CloseHandle(Snapshot);
		return NULL;
	}

	std::wstring GetFullPath(DWORD ProcessId) {


		HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessId);

		MODULEENTRY32 Me32;
		Me32.dwSize = sizeof(Me32);

		if (Module32First(Snapshot, &Me32)) {
			do {

				if (!wcscmp(Me32.szModule, L"Arma3_x64.exe")) {

					CloseHandle(Snapshot);

					return std::wstring(Me32.szExePath);
				}

			} while (Module32Next(Snapshot, &Me32));
		}

		CloseHandle(Snapshot);
		return L"";
	}

	bool Init() {
		DWORD ProcessID = GetArmaProcessID();

		std::wstring FullPath = GetFullPath(ProcessID);

		printf("FullPath: %ws\n", FullPath.c_str());

		const auto ModuleBase = (UINT64)LoadLibraryW(FullPath.c_str());

		const auto Dos = (PIMAGE_DOS_HEADER)(ModuleBase);

		if (!Dos)
			return 0;

		const auto Nt = (PIMAGE_NT_HEADERS)(ModuleBase + Dos->e_lfanew);

		auto Section = IMAGE_FIRST_SECTION(Nt);

		UINT64 TextSectionBase = 0;
		UINT64 TextSectionSize = 0;

		for (int i = 0; i < Nt->FileHeader.NumberOfSections; i++, Section++) {

			if (!Section->VirtualAddress || !Section->SizeOfRawData)
				continue;

			printf("Section: %s\n", Section->Name);

			if (!strcmp((const char*)Section->Name, ".text")) {
				TextSectionBase = ModuleBase + Section->VirtualAddress;
				TextSectionSize = Section->SizeOfRawData;
				break;
			}
		}
		printf("Text Section Start: 0x%llX \n", TextSectionBase);
		printf("Text Section Size: 0x%llX \n", TextSectionSize);

		m_TextBufferOffset = Section->VirtualAddress;

		m_TextBuffer = new BYTE[TextSectionSize];

		memcpy(m_TextBuffer, (void*)TextSectionBase, TextSectionSize);
		
		//NewCompare(m_TextBuffer, TextSectionSize, "\xe8\x00\x00\x00\x00\x48\x83\xc4\x00\xe9", "x????xxx?x");
		//FindSignature(m_TextBuffer, TextSectionBase, TextSectionSize, Signatures::ModBase::World::pattern, Signatures::ModBase::World::mask);
		for (auto* sig : GetSigList()) {

			*sig->outAddress = FindSignature(m_TextBuffer, TextSectionSize, *sig);

			if (*sig->outAddress) {
				printf("found a signature");
				CheckType(sig);
			}
			else {
				printf("failed to find a signature");
			}

		}

		FreeLibrary((HMODULE)ModuleBase); // Free after use.
		return 1;
	}
	/*
	const BYTE* NewCompare(const BYTE* Buffer, UINT64 BufferSize, const char* Sig, const char* Mask) {
		for (int i = 0; i < BufferSize - strlen(Sig); i++) {
			bool found = true;
			for (int x = 0; x < strlen(Sig); x++) {
				if (Mask[x] != '?' && Sig[x] != *(Buffer + i + x));
				{
					found = false;
					break;
				}
			}
			if (found) {
				return Buffer + i;
			}
		}
	}*/

	bool MemoryCompare(const BYTE* bData, const BYTE* bMask, const char* szMask) {
		for (; *szMask; ++szMask, ++bData, ++bMask) {
			if (*szMask == 'x' && *bData != *bMask) {
				return false;
			}
		}
		return (*szMask == NULL);
	}

	BYTE* FindSignature(BYTE* start, DWORD size, SignatureDef sig)
	{

		for (DWORD i = 0; i < size; i++)
		{
			if (MemoryCompare(start + i, (const BYTE*)sig.pattern, sig.mask)) {
				return start + i;
			}
		}
		
		return NULL;
	}


	void CheckType(SignatureDef* sig) {

		switch (sig->ST)
		{
		case WORLD:
			*sig->outValue = FormatSig0(sig);
			break;
		case PLAYER:
			*sig->outValue = FormatSig1(sig);
			break;
		default:
			break;
		}

	}

	UINT32 FormatSig0(SignatureDef* sig) {
		const auto offsetLocation = (BYTE*)(*sig->outAddress) + 3;
		const auto offset = *(int*)(offsetLocation);

		return ((*sig->outAddress + offset + 7) + m_TextBufferOffset) - m_TextBuffer; // cant just do textbufferLocation + offset
																										// need to get the offset relative from our code. Read more about this.
																										// address of text buffer is not the same as the games textbuffer even tho we copied it over
	}

	UINT32 FormatSig1(SignatureDef* sig) {

		return *(int*) (*sig->outAddress + 3);
	}



};

