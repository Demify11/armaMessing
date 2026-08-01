#include "Group.h"
#include "imgui.h"

float Group::Height(){
    return m_Measured > 0.f ? m_Measured : m_Provisional;
}

void Group::Draw(float width = 0.f) {
    ImGui::BeginChild(m_Name.c_str(), ImVec2(width, Height()), true); //manually pass in m_Height or use new Height()

    ImGui::TextUnformatted(m_Name.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    int i = 0;
    for (auto* el : m_Elements) {
        ImGui::PushID(i++);
        el->Draw();
        ImGui::PopID();
    }

    m_Measured = ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y;

    ImGui::EndChild();
}
