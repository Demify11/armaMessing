#pragma once
#pragma once
#include "imgui.h"

struct FontSet {
    ImFont* Body = nullptr;
    ImFont* Title = nullptr;
    ImFont* Icons = nullptr;
};
inline FontSet g_Fonts;

void LoadFonts();

namespace Palette {
    constexpr ImU32  PanelBg = IM_COL32(29, 30, 34, 255);
    constexpr ImU32  HeaderBg = IM_COL32(19, 20, 22, 255);
    constexpr ImU32  CardBg = IM_COL32(24, 25, 27, 255);
    constexpr ImU32  TextMuted = IM_COL32(83, 86, 93, 255);
    constexpr ImVec4 Accent = ImVec4(0.66f, 0.20f, 0.92f, 1.0f);
}

namespace Layout {
    constexpr float Rounding = 10.f;
    constexpr float HeaderH = 48.f;
    constexpr float SidebarW = 54.f;
    constexpr float ContentPad = 12.f;
    constexpr float TabButton = 34.f;
}