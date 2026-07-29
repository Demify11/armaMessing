#include "Form.h"
#include "imgui.h"

Tab& Form::AddTab(std::string icon, std::string name) {
    return m_Tabs.emplace_back(std::move(icon), std::move(name));
}

void Form::Draw()
{
    ImGui::Begin("menu");

    ImGui::BeginChild("##sidebar", ImVec2(50, 0), true);

    for (int i = 0; i < m_Tabs.size(); i++) {
        const bool selected = (i == m_Selected);
        ImGui::PushID(i);

        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.20f, 1.f));

        if (ImGui::Button(m_Tabs[i].Icon().c_str(), ImVec2(31, 31)))
            m_Selected = i;

        if (selected)
            ImGui::PopStyleColor();

        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // --- page for the selected tab ---
    ImGui::BeginChild("##page", ImVec2(0, 0), false); // fill remaining space
    if (m_Selected >= 0 && m_Selected < (int)m_Tabs.size())
        m_Tabs[m_Selected].DrawPage();
    ImGui::EndChild();

    ImGui::End();
}

