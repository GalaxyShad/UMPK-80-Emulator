#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

class UiMemoryDump : public IRenderable {
public:
    UiMemoryDump(std::vector<uint8_t>& memory) : m_memory(memory) {}

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

        for (int row = 0; row < m_memory.size() / 16; row++) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TableSetBgColor(
                ImGuiTableBgTarget_CellBg,
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]), 0
            );

            
            ImGui::Text("%04X", row << 4);

            for (int column = 0; column < 16; column++) {
                ImGui::TableSetColumnIndex(column+1);
                
                // ImGui::Selectable()

                ImGui::Text("%02X", m_memory[row * 16 + column]);
            }
        }
        ImGui::EndTable();
    }
private:
    std::vector<uint8_t>& m_memory;
};