#pragma once

#include "imgui.h"

namespace UI {
    // Draws an icon tab in the sidebar. Returns true on click.
    bool AccentTab(const char* str_id, const char* icon, bool selected, ImVec2 size);
    bool Checkbox(const char* label, bool* v);
    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt = "%.2f");
    bool Button(const char* label, ImVec2 size = ImVec2(0, 0));
}

