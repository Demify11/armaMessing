#include "Group.h"
#include "Fonts.h"
#include "imgui.h"


float Group::Height(){
    return m_Measured > 0.f ? m_Measured : m_Provisional;
}

void Group::Draw(float width = 0.f) {

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();   // screen-space top-left of the card
    const float  h = Height();

    // ---- card background (drawn first, sits behind the contents) ----
    dl->AddRectFilled(p, ImVec2(p.x + width, p.y + h), Palette::CardBg, 8.0f);
    // hairline top edge for a touch of depth (optional)
    dl->AddRect(p, ImVec2(p.x + width, p.y + h), IM_COL32(255, 255, 255, 10), 8.0f);

    // transparent bg (ChildBg alpha 0), no border — the visual frame is our rect above.
    ImGui::SetCursorScreenPos(p);
    ImGui::BeginChild(m_Name.c_str(), ImVec2(width, h), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const float pad = 12.0f;
    ImGui::Dummy(ImVec2(0, pad - 4));

    {
        ImGui::SetCursorPosX(pad);
        const ImVec2 tickPos = ImGui::GetCursorScreenPos();
        dl->AddRectFilled(ImVec2(tickPos.x, tickPos.y + 2),
            ImVec2(tickPos.x + 3, tickPos.y + ImGui::GetTextLineHeight() + 2),
            ImGui::GetColorU32(Palette::Accent), 1.5f);
        ImGui::SetCursorPosX(pad + 10);          // nudge title right of the tick
        ImGui::PushFont(g_Fonts.Title);
        ImGui::TextUnformatted(m_Name.c_str());
        ImGui::PopFont();
    }
    ImGui::Dummy(ImVec2(0, 6));

    ImGui::Indent(pad);
    ImGui::PushItemWidth(width - pad * 2);
    int i = 0;
    for (auto& el : m_Elements) {
        ImGui::PushID(i++);
        el->Draw();
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    ImGui::Unindent(pad);

    // ---- measure: where did the content pen end up (local Y) + bottom padding ----
    m_Measured = ImGui::GetCursorPosY() + pad;

    ImGui::EndChild();

}
