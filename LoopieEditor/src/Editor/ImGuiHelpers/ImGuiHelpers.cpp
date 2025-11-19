
#include "ImGuiHelpers.h"

#include <cstddef>

namespace Loopie {
 
    std::string ImGuiHelpers::TruncateText(const std::string& text, float width)
    {
        std::string truncated_text = text;

        const float text_width = ImGui::CalcTextSize(text.c_str(), nullptr, true).x;

        if (text_width > width) {
            constexpr const char* ELLIPSIS = "...";
            const float ellipsis_size = ImGui::CalcTextSize(ELLIPSIS).x;

            size_t visible_chars = 0;
            for (size_t i = 0; i < text.size(); i++) {
                const float current_width = ImGui::CalcTextSize(
                    text.substr(0, i).c_str(), nullptr, true)
                    .x;
                if (current_width + ellipsis_size > width) {
                    break;
                }
                visible_chars = i;
            }
            truncated_text = (text.substr(0, visible_chars) + ELLIPSIS).c_str();
        }
        return truncated_text;
    }

    void ImGuiHelpers::TextCentered(const std::string& text, float align)
    {
        const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        const ImVec2 avail = ImGui::GetContentRegionAvail();

        float posX = ImGui::GetCursorPosX() + (avail.x - textSize.x) * align;
        ImGui::SetCursorPosX(posX);
        ImGui::Text(text.c_str());
    }

#pragma region Splitter
    Splitter::Splitter(SplitterMode mode, float ratio, float size, float rounding, int color)
        : m_mode(mode), m_ratio(ratio), m_size(size), m_rounding(rounding), m_colorStyle(color) {

    }

    void Splitter::Place(const ImVec2& avail) {
        bool vertical = (m_mode == VERTICAL);

        if(vertical)
            ImGui::SameLine();

        if (avail.x == 0 || avail.y == 0)
            return;

        ImGui::InvisibleButton("splitter", (vertical ? ImVec2(m_size, avail.y ) : ImVec2(avail.x, m_size)));
        if (ImGui::IsItemActive()) {
            float mouseDelta = vertical ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y;
            m_ratio += mouseDelta / (vertical ? avail.x : avail.y);
        }

        float minRatio = 30.0f / (vertical ? avail.x : avail.y);
        float maxRatio = 1.0f - minRatio;

        if (m_ratio < minRatio) m_ratio = minRatio;
        if (m_ratio > maxRatio) m_ratio = maxRatio;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 splitterMin = ImGui::GetItemRectMin();
        ImVec2 splitterMax = ImGui::GetItemRectMax();

        auto& style = ImGui::GetStyle();
        ImVec4& color = style.Colors[m_colorStyle];
        draw_list->AddRectFilled(splitterMin, splitterMax, IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 255), m_rounding);

        if (vertical)
            ImGui::SameLine();
    }

    void Splitter::GetSizes(const ImVec2& avail, float& sizeFirst, float& sizeSecond) const {
        bool vertical = (m_mode == VERTICAL);

        sizeFirst = (vertical ? avail.x : avail.y) * m_ratio;
        sizeSecond = (vertical ? avail.x : avail.y) - sizeFirst - m_size;
    }
#pragma endregion


}