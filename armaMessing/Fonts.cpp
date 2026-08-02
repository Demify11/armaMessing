#include "Fonts.h"
#include "IconsFontAwesome5.h"

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();

    g_Fonts.Body = io.Fonts->AddFontFromFileTTF("Inter_18pt-Medium.ttf", 16.0f);

    g_Fonts.Title = io.Fonts->AddFontFromFileTTF("Inter_18pt-SemiBold.ttf", 18.0f);

    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = false; // standalone, so we can PushFont it
    icons_config.PixelSnapH = true;
    g_Fonts.Icons = io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 20.0f, &icons_config, icon_ranges);
}