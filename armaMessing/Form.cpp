#pragma once
#include "Form.h"
#include "imgui.h"
#include "Widgets.h"
#include "Fonts.h"

Tab& Form::AddTab(std::string icon, std::string name) {
    return m_Tabs.emplace_back(std::move(icon), std::move(name));
}

void Form::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::SetNextWindowSize(ImVec2(SizeX, SizeY));
    ImGui::Begin("##menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetWindowPos();
    const ImVec2 sz = ImGui::GetWindowSize();
    const float  R = Layout::Rounding;

    //dl->AddRectFilled(ImVec2(p.x - 6, p.y - 6), ImVec2(p.x + sz.x + 6, p.y + sz.y + 6), IM_COL32(0, 0, 0, 40), Layout::Rounding + 4);

    dl->AddRectFilled(p, ImVec2(p.x + sz.x, p.y + sz.y), Palette::PanelBg, R);

    dl->AddRectFilled(p, ImVec2(p.x + sz.x, p.y + Layout::HeaderH), Palette::HeaderBg, R, ImDrawFlags_RoundCornersTop);

    dl->AddRectFilled(ImVec2(p.x, p.y + Layout::HeaderH),ImVec2(p.x + Layout::SidebarW, p.y + sz.y),Palette::HeaderBg, R, ImDrawFlags_RoundCornersBottomLeft);

    // subtle depth hairlines
    dl->AddLine(ImVec2(p.x + Layout::SidebarW, p.y + Layout::HeaderH), ImVec2(p.x + Layout::SidebarW, p.y + sz.y), IM_COL32(0, 0, 0, 90));
    dl->AddLine(ImVec2(p.x + Layout::SidebarW, p.y + Layout::HeaderH), ImVec2(p.x + sz.x, p.y + Layout::HeaderH), IM_COL32(0, 0, 0, 90));

    ImGui::PushFont(g_Fonts.Title);
    ImGui::SetCursorPos(ImVec2(Layout::SidebarW + Layout::ContentPad,
        (Layout::HeaderH - ImGui::GetFontSize()) * 0.5f));
    ImGui::TextUnformatted("0Lab");
    ImGui::PopFont();

    ImGui::PushFont(g_Fonts.Icons);
    {
        float tabY = Layout::HeaderH + 12.f;
        for (int i = 0; i < (int)m_Tabs.size(); i++) {
            ImGui::PushID(i);
            ImGui::SetCursorPos(ImVec2((Layout::SidebarW - Layout::TabButton) * 0.5f, tabY));

            if (UI::AccentTab("tab", m_Tabs[i].Icon().c_str(),
                i == m_Selected,
                ImVec2(Layout::TabButton, Layout::TabButton)))
                m_Selected = i;

            ImGui::PopID();
            tabY += Layout::TabButton + 10.f;
        }
    }
    ImGui::PopFont();

    // ---- content area ----
    ImGui::SetCursorPos(ImVec2(Layout::SidebarW + Layout::ContentPad,
        Layout::HeaderH + Layout::ContentPad));
    ImGui::BeginChild("##content",ImVec2(sz.x - Layout::SidebarW - Layout::ContentPad * 2.f,sz.y - Layout::HeaderH - Layout::ContentPad * 2.f), false, ImGuiWindowFlags_NoScrollbar);

    if (m_Selected >= 0 && m_Selected < (int)m_Tabs.size())
        m_Tabs[m_Selected].DrawPage();

    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleVar();

    ImTricks::NotifyManager::HandleNotifies(ImGui::GetForegroundDrawList());
}

