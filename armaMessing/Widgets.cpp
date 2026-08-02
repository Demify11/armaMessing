#pragma once
#include "Widgets.h"
#include "Fonts.h"
#include <unordered_map>
#include "imgui_internal.h"


    // One eased value per ImGuiID. Relies on ImGui IDs being unique, which the
    // PushID(i) loop in Group::Draw guarantees per element.
    float& AnimFor(ImGuiID id) {
        static std::unordered_map<ImGuiID, float> s;
        return s[id];
    }
    // Eases `a` toward `target` a fraction per frame (frame-rate independent via exp).
    // NOTE the ease is asymptotic: it never mathematically *lands* on its target,
    // it just gets ever-closer. That's fine at the top (0.99998 alpha rounds to 255,
    // looks identical to 1.0, and nothing branches on hitting 1.0 exactly).
    // It's NOT fine at the bottom: idle is the long-lived state and code keys off it
    // (e.g. `if (a > 0.0f)` skips the draw call for idle widgets). A lingering
    // 0.0000004 would keep that true forever. So we snap to a clean 0 — but ONLY when
    // target==0 (fading out & basically gone), so we don't yank down a fade that's on
    // its way UP through the same tiny zone.
    float EaseTo(float& a, float target, float speed) {
        a += (target - a) * (1.0f - expf(-speed * ImGui::GetIO().DeltaTime));
        if (target == 0.f && a < 0.0015f) a = 0.f;
        return a;
    }


namespace UI {

    bool Checkbox(const char* label, bool* v) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const float  box = 18.0f;
        const float  gap = 9.0f;
        const ImVec2 lblSz = ImGui::CalcTextSize(label);

        // full-width row is clickable, not just the box
        const float rowW = ImMax(box + gap + lblSz.x, ImGui::GetContentRegionAvail().x);
        const bool clicked = ImGui::InvisibleButton(label, ImVec2(rowW, box));
        const bool hovered = ImGui::IsItemHovered();
        if (clicked) *v = !*v;

        float& a = AnimFor(window->GetID(label));
        EaseTo(a, *v ? 1.f : 0.f, 16.f);

        ImDrawList* dl = window->DrawList;
        const ImVec2 b1(pos.x + box, pos.y + box);

        // box: lerp dark frame -> accent
        dl->AddRectFilled(pos, b1,
            ImGui::GetColorU32(ImLerp(ImVec4(0.14f, 0.15f, 0.17f, 1.f), Palette::Accent, a)), 5.f);
        if (hovered)
            dl->AddRect(pos, b1, ImGui::GetColorU32(ImVec4(1, 1, 1, 0.25f)), 5.f);

        // checkmark, alpha driven by a
        if (a > 0.01f) {
            const ImU32 ck = ImGui::GetColorU32(ImVec4(1, 1, 1, a));
            dl->AddLine(ImVec2(pos.x + box * 0.26f, pos.y + box * 0.52f),
                ImVec2(pos.x + box * 0.44f, pos.y + box * 0.70f), ck, 2.f);
            dl->AddLine(ImVec2(pos.x + box * 0.44f, pos.y + box * 0.70f),
                ImVec2(pos.x + box * 0.76f, pos.y + box * 0.30f), ck, 2.f);
        }

        dl->AddText(ImVec2(pos.x + box + gap, pos.y + (box - lblSz.y) * 0.5f),
            ImGui::GetColorU32(ImVec4(0.86f, 0.86f, 0.88f, 1.f)), label);
        return clicked;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* fmt) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImDrawList* dl = window->DrawList;
        const float w = ImGui::CalcItemWidth();          // respects PushItemWidth
        const float lblH = ImGui::GetTextLineHeight();
        const float grabR = 7.f;
        const float rowH = lblH + 10.f + grabR * 2.f;

        const ImVec2 pos = ImGui::GetCursorScreenPos();

        // whole-row hit target — reserves space AND lets you drag anywhere on the row
        const bool  pressed = ImGui::InvisibleButton(label, ImVec2(w, rowH));
        const bool  active = ImGui::IsItemActive();
        const bool  hovered = ImGui::IsItemHovered();

        bool changed = false;
        if (active) {
            float t = (ImGui::GetIO().MousePos.x - pos.x) / w;
            t = t < 0 ? 0 : (t > 1 ? 1 : t);
            float nv = v_min + t * (v_max - v_min);
            if (nv != *v) { *v = nv; changed = true; }
        }

        // label (left) + value (right), on the top line
        char buf[64]; snprintf(buf, sizeof(buf), fmt, *v);
        const ImVec2 valSz = ImGui::CalcTextSize(buf);
        dl->AddText(pos, ImGui::GetColorU32(ImVec4(0.86f, 0.86f, 0.88f, 1.f)), label);
        dl->AddText(ImVec2(pos.x + w - valSz.x, pos.y),
            ImGui::GetColorU32(Palette::Accent), buf);

        // track
        const float ty = pos.y + lblH + 10.f + grabR;
        const ImVec2 t0(pos.x, ty - 2.f);
        const ImVec2 t1(pos.x + w, ty + 2.f);
        dl->AddRectFilled(t0, t1, ImGui::GetColorU32(ImVec4(0.18f, 0.19f, 0.21f, 1.f)), 2.f);

        float frac = (*v - v_min) / (v_max - v_min);
        frac = frac < 0 ? 0 : (frac > 1 ? 1 : frac);
        const float gx = pos.x + frac * w;
        dl->AddRectFilled(t0, ImVec2(gx, t1.y), ImGui::GetColorU32(Palette::Accent), 2.f);

        dl->AddCircleFilled(ImVec2(gx, ty), grabR,
            ImGui::GetColorU32((active || hovered) ? ImVec4(1, 1, 1, 1) : ImVec4(0.9f, 0.9f, 0.95f, 1.f)));

        return changed;
    }

    bool Button(const char* label, ImVec2 size) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        const ImVec2 lblSz = ImGui::CalcTextSize(label);
        if (size.x <= 0) size.x = lblSz.x + 24.f;
        if (size.y <= 0) size.y = lblSz.y + 12.f;

        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const bool clicked = ImGui::InvisibleButton(label, size);
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();

        float& a = AnimFor(window->GetID(label));
        EaseTo(a, held ? 1.f : (hovered ? 0.6f : 0.35f), 16.f);

        ImDrawList* dl = window->DrawList;
        ImVec4 col = Palette::Accent; col.w = a;
        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(col), 6.f);
        dl->AddText(ImVec2(pos.x + (size.x - lblSz.x) * 0.5f, pos.y + (size.y - lblSz.y) * 0.5f),
            ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), label);
        return clicked;
    }

    bool AccentTab(const char* str_id, const char* icon, bool selected, ImVec2 size)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        // Screen-space anchor for our own drawing, taken from the current pen.
        const ImVec2 pos = ImGui::GetCursorScreenPos();

        // Hit-testing: an invisible button reserves the rect and gives us click/hover/active.
        const bool clicked = ImGui::InvisibleButton(str_id, size);
        const bool hovered = ImGui::IsItemHovered();

        // ---- animation: one float per widget, eased toward a target every frame ----
        // `a` is a REFERENCE into the anim map (keyed by widget ID) — writes persist to
        // next frame. It drives highlight alpha: 0 = idle/invisible, 1 = full accent.
        float& a = AnimFor(window->GetID(str_id));
        EaseTo(a, selected ? 1.0f : (hovered ? 0.45f : 0.0f), 14.0f);

        ImDrawList* dl = window->DrawList;

        // filled rounded highlight behind the icon (accent, alpha driven by 'a')
        if (a > 0.0f) {
            ImVec4 col = Palette::Accent; col.w = a;
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(col), 8.0f);
        }

        // bar on the left edge when selected
        if (selected) {
            const float barH = size.y * 0.55f;
            const ImVec2 b0(pos.x - 4.0f, pos.y + (size.y - barH) * 0.5f);
            const ImVec2 b1(pos.x - 1.0f, b0.y + barH);
            ImVec4 acc = Palette::Accent; acc.w = a;
            dl->AddRectFilled(b0, b1, ImGui::GetColorU32(acc), 2.0f);
        }

        // the icon glyph, centered; brightens from muted grey to white rises
        const ImU32 icol = ImGui::GetColorU32(ImLerp(ImVec4(0.55f, 0.55f, 0.60f, 1.f), ImVec4(1, 1, 1, 1), a));
        const ImVec2 ts = ImGui::CalcTextSize(icon);
        dl->AddText(ImVec2(pos.x + (size.x - ts.x) * 0.5f, pos.y + (size.y - ts.y) * 0.5f), icol, icon);

        return clicked;
    }

} // namespace UI