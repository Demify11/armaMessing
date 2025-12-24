#pragma once
#include "Framework.h"

auto Thread::StartSpoofedThread(PVOID Destination) -> DWORD {
	const auto Library = LoadLibraryA("ntdll.dll");

	const auto ApiAddress = GetProcAddress(Library, "RtlRandomEx");

	if (!ApiAddress)
		return 0;

	// Prepare Hook
	const BYTE Payload[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	const BYTE Original[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// Point to Destination
	*(PVOID*)(&Payload[6]) = Destination;

	// Change RWX Memory
	DWORD OldProtect;

	if (!VirtualProtect((LPVOID)*ApiAddress, 15, PAGE_EXECUTE_READWRITE, &OldProtect))
		return 0;

	// Copy Original in
	memcpy((PVOID)Original, ApiAddress, sizeof(Payload));

	// Copy Hook In
	memcpy(ApiAddress, Payload, sizeof(Payload));

	// Start Thread in target
	HANDLE Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ApiAddress, 0, 0, 0);

	if (!Thread)
		return 0;

	// Wait
	Sleep(1500);

	// Place Original back
	memcpy(ApiAddress, Original, sizeof(Payload));

	// Place back old protection
	if (!VirtualProtect((LPVOID)*ApiAddress, 15, PAGE_EXECUTE_READWRITE, &OldProtect))
		return 0;

	DWORD ThreadId = GetThreadId(Thread);

	CloseHandle(Thread);

	return ThreadId;
}