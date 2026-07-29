#pragma once
#include "imgui.h"
#include <string>
#include <functional>
class Element {
public:
    virtual ~Element() = default;
    virtual void Draw() = 0;
};

class Button : public Element {
    std::string m_Label;
    std::function<void()> m_OnClick;
public:
    Button(std::string label, std::function<void()> onClick)
        : m_Label(std::move(label)), m_OnClick(std::move(onClick)) {}
    void Draw() override {
        if (ImGui::Button(m_Label.c_str()) && m_OnClick) m_OnClick();
    }
};

class Text : public Element {
    std::string m_Text;
public:
    explicit Text(std::string text) : m_Text(std::move(text)) {}
    void Draw() override { ImGui::TextUnformatted(m_Text.c_str()); }
};