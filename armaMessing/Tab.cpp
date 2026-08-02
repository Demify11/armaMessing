#include "Tab.h"
#include "imgui_internal.h"

Group& Tab::AddGroup(std::string name) {
    return m_Groups.emplace_back(std::move(name));
}

void Tab::DrawPage() {
    const float gap = 8.f;
    const float startX = ImGui::GetCursorPosX();
    const float startY = ImGui::GetCursorPosY();
    const float colW = (ImGui::GetContentRegionAvail().x - gap) / 2.f;

    float leftY = startY; // These are the "pens" for the 2 colums, anton code
    float rightY = startY;

    int index = 0;
    for (auto& g : m_Groups) {
        const bool  left = (index % 2 == 0);
        const float x = left ? startX : startX + colW + gap; //better if statements
        float& y = left ? leftY : rightY;   //right pen

        ImGui::SetCursorPos(ImVec2(x, y));
        g.Draw(colW);

        y += g.Height() + gap;
        ++index;
    }

    // drop the cursor below the taller column so later content doesn't overlap
    ImGui::SetCursorPosY(leftY > rightY ? leftY : rightY);
}
