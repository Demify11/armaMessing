#include "Group.h"
#include "imgui.h"


void Group::Draw(float width = 0.f) {
    ImGui::BeginChild(m_Name.c_str(), ImVec2(width, m_Height), true);

    ImGui::TextUnformatted(m_Name.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    int i = 0;
    for (auto* el : m_Elements) {
        ImGui::PushID(i++);
        el->Draw();
        ImGui::PopID();
    }

    ImGui::EndChild();
}
