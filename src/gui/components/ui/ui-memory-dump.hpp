#ifndef UI_MEMORY_DUMP_HPP
#define UI_MEMORY_DUMP_HPP

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cstring>
#include <functional>
#include <imgui.h>
#include <string>

#include "../irenderable.hpp"

class UiMemoryDump : public IRenderable {
public:
    UiMemoryDump(const uint8_t *arr, size_t arrSize, int startLabel = 0)
        : m_arrSize(arrSize), m_startLabel(startLabel), m_readonly(true) {
        m_arr = new uint8_t[arrSize];
        memcpy(m_arr, arr, arrSize);
    }

    UiMemoryDump(
        size_t arrSize,
        std::function<void(uint64_t index, uint8_t value)> changeCallback,
        int startLabel = 0)
        : m_arrSize(arrSize), m_startLabel(startLabel), m_readonly(false),
          m_changeCallback(changeCallback) {
        m_arr = new uint8_t[arrSize];
        memset(m_arr, 0, arrSize);
    }

    ~UiMemoryDump() {
        delete[] m_arr;
        m_arr = nullptr;
    }

    void update(const uint8_t *newArr, size_t newSize) {
        delete[] m_arr;

        m_arr = new uint8_t[newSize];
        memcpy(m_arr, newArr, newSize);

        m_arrSize = newSize;
    }

    void render() override {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,
                            (m_readonly) ? ImVec2(2, 0) : ImVec2(0, 0));
        if (ImGui::BeginTable("Dump", 17,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_ScrollX |
                                  ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedSame)) {

            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableSetupColumn("");
            for (int column = 0; column < 16; column++)
                ImGui::TableSetupColumn(
                    ("0" + std::string(1, "0123456789ABCDEF"[column])).c_str());
            ImGui::TableHeadersRow();

            for (int row = 0; row < m_arrSize / 16 + 1; row++) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TableSetBgColor(
                    ImGuiTableBgTarget_CellBg,
                    ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]),
                    0);

                ImGui::Text("%04X", m_startLabel + (row << 4));

                for (int column = 0; column < ((m_arrSize < 16) ? m_arrSize : 16); column++) {
                    ImGui::TableSetColumnIndex(column + 1);

                    // ImGui::Selectable()
                    int index = row * 16 + column;

                    if (m_readonly) {
                        ImGui::Text("%02X", m_arr[index]);
                    } else {
                        ImGui::PushItemWidth(-1.0);

                        if (ImGui::InputScalar(
                                ("##r" + std::to_string((index))).c_str(),
                                ImGuiDataType_U8, &m_arr[index], NULL, NULL,
                                "%02X")) {

                            m_changeCallback(index, m_arr[index]);
                        }
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

private:
    uint8_t *m_arr;
    size_t m_arrSize;
    int m_startLabel;
    bool m_readonly;
    std::function<void(uint64_t index, uint8_t value)> m_changeCallback;
};

#endif // UI_MEMORY_DUMP_HPP
