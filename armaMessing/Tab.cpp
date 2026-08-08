#include "Tab.h"
#include "imgui_internal.h"

Group& Tab::AddGroup(std::string name) {
    return m_Groups.emplace_back(std::move(name));
}

void Tab::DrawPage() {
    const float gap = 8.f;
    const float startX = ImGui::GetCursorPosX();
    const float startY = ImGui::GetCursorPosY();
    const float availW = ImGui::GetContentRegionAvail().x;
    const float colW = (availW - gap) / 2.f;

    float leftY = startY; // These are the "pens" for the 2 colums, anton code
    float rightY = startY;

    int index = 0;
    for (auto& g : m_Groups) {
        if (g.IsFullWidth()) { //for full width groups
            float y = leftY > rightY ? leftY : rightY;
            ImGui::SetCursorPos(ImVec2(startX, y));
            g.Draw(availW);
            y += g.Height() + gap;
            leftY = y;
            rightY = y;
            index = 0;
        }
        else {
            const bool  left = (index % 2 == 0);
            const float x = left ? startX : startX + colW + gap; //better if statements
            float& y = left ? leftY : rightY;   //right pen

            ImGui::SetCursorPos(ImVec2(x, y));
            g.Draw(colW);

            y += g.Height() + gap;
            ++index;
        }
    }

    // drop the cursor below the taller column so later content doesn't overlap
    ImGui::SetCursorPosY(leftY > rightY ? leftY : rightY);
}
