#pragma once

enum ScanType {
    WORLD,
    PLAYER,
    ENTITYLIST

};

namespace Offsets {
    inline uintptr_t World = 0;
    inline uintptr_t LocalPlayer = 0;
    inline uintptr_t EntityListNear = 0;
}

struct SignatureDef {
    const char* name;
    const char* pattern;
    const char* mask;
    BYTE** outAddress;
    ScanType ST;
    uintptr_t* outValue = nullptr;
}; //for future can use this structure to make a list of sigs and then loop thru them to find signatures automatically

inline std::vector<SignatureDef*>& GetSigList() {
    static std::vector<SignatureDef*> Sigs;
    return Sigs;
}

struct SigReg {
    SigReg(SignatureDef* def) {
        GetSigList().push_back(def);
    }
};

#define ADD_SIGNATURE(klass, name, patternStr, maskStr, type, outPtr) \
namespace klass { \
    struct name { \
        inline static const char* pattern = patternStr; \
        inline static const char* mask = maskStr; \
        inline static BYTE* address = 0; \
        inline static SignatureDef def = { #name, pattern, mask, &address, type, outPtr}; \
        inline static SigReg reg {&def}; \
    };\
}

namespace Signatures {
        ADD_SIGNATURE(ModBase, World,
            "\x48\x8B\x05\x00\x00\x00\x00\x45\x33\xC0\x48\x8B\xD9\x48\x8B\x90\x00\x00\x00\x00\x48\x8B\x52\x08\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3",
            "xxx????xxxxxxxxx????xxxxx????xxxxxxxxx", WORLD, &Offsets::World);
        ADD_SIGNATURE(World, LocalPlayer, "\x48\x8B\x90\x00\x00\x00\x00\x48\x8B\x52\x08\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3\xCC\x40\x53",
            "xxx????xxxxx????xxxxxxxxxxxx", PLAYER, &Offsets::LocalPlayer);
        ADD_SIGNATURE(World, EntityListNear, "\x4C\x63\xB6\x00\x00\x00\x00\x44\x88\x65\xD8\x4D\x85\xF6\x0F\x8E\x00\x00\x00\x00\x49\x8B\xFC\x4C\x8D\x2D\x00\x00\x00\x00\x0F\x1F\x80\x00\x00\x00\x00\x48\x8B\x86\x00\x00\x00\x00\x48\x8B\x1C\xF8",
            "xxx????xxxxxxxxx????xxxxxx????xxx????xxx????xxxx", ENTITYLIST, &Offsets::EntityListNear);


}

