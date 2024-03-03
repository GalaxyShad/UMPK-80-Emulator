#ifndef UI_MEMORY_DUMP_HPP
#define UI_MEMORY_DUMP_HPP

#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

class UiMemoryDump : public IRenderable {
public:
    UiMemoryDump(const uint8_t* arr, size_t arrSize, int startLabel = 0) : m_arrSize(arrSize), m_arr(arr), m_startLabel(startLabel) {}

    void update(const uint8_t* newArr, size_t newSize) { m_arr = newArr; m_arrSize = newSize; }

    void render() override {
        if (!ImGui::BeginTable(
            "ROM and RAM", 
            17, 
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
        )) return;

        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("");
        for (int column = 0; column < 16; column++)
            ImGui::TableSetupColumn(("0" + std::string(1, "0123456789ABCDEF"[column])).c_str());
        ImGui::TableHeadersRow();

        for (int row = 0; row < m_arrSize / 16; row++) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TableSetBgColor(
                ImGuiTableBgTarget_CellBg,
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]), 0
            );

            ImGui::Text("%04X", m_startLabel + (row << 4));

            for (int column = 0; column < 16; column++) {
                ImGui::TableSetColumnIndex(column+1);
                
                // ImGui::Selectable()
                
                ImGui::Text("%02X", m_arr[row * 16 + column]);
            }
        }
        ImGui::EndTable();
    }
private:
    const uint8_t* m_arr;
    size_t m_arrSize;
    int m_startLabel;
};

#endif // UI_MEMORY_DUMP_HPP
