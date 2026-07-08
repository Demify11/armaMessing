#pragma once
#include <cstdint>
#include <Windows.h>

#pragma pack(push, 1)
struct LicenseContext {
    char refreshToken[512];
    char licenseId[64];
    char fingerprint[128];
    char processId[64];
    int32_t interval;        // seconds
};
#pragma pack(pop)
