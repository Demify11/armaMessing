#pragma once
#include "imgui.h"
#include <string>
#include <functional>
#include "Widgets.h"

class Element {
public:
    virtual ~Element() = default;
    virtual void Draw() = 0;
};

class Button : public Element {
    std::string m_Label; std::function<void()> m_OnClick;
public:
    Button(std::string l, std::function<void()> cb)
        : m_Label(std::move(l)), m_OnClick(std::move(cb)) {}
    void Draw() override { if (UI::Button(m_Label.c_str()) && m_OnClick) m_OnClick(); }
};

class Text : public Element {
    std::string m_Text;
public:
    explicit Text(std::string t) : m_Text(std::move(t)) {}
    void Draw() override { ImGui::TextUnformatted(m_Text.c_str()); }
};

class Checkbox : public Element {
    std::string m_Label; bool* m_Value;
public:
    Checkbox(std::string l, bool* v) : m_Label(std::move(l)), m_Value(v) {}
    void Draw() override { UI::Checkbox(m_Label.c_str(), m_Value); }
};

class SliderFloat : public Element {
    std::string m_Label; float* m_Value; float m_Min, m_Max;
public:
    SliderFloat(std::string l, float* v, float mn, float mx)
        : m_Label(std::move(l)), m_Value(v), m_Min(mn), m_Max(mx) {}
    void Draw() override { UI::SliderFloat(m_Label.c_str(), m_Value, m_Min, m_Max); }
};