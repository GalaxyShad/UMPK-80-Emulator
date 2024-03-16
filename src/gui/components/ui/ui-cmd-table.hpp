#ifndef UI_CMD_TABLE_HPP
#define UI_CMD_TABLE_HPP

#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

#include "../../../core/disassembler.hpp"

class UiCmdTable : public IRenderable {
public:
    void render() override {
        if (!ImGui::BeginTable(
            "UiCmdTable", 
            17, 
            ImGuiTableFlags_Borders | 
            ImGuiTableFlags_RowBg   | 
            ImGuiTableFlags_ScrollX | 
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedSame
        )) return;

        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("");

        for (int column = 0; column < 16; column++) {
            ImGui::TableSetupColumn(("0" + std::string(1, "0123456789ABCDEF"[column])).c_str());
        }

        ImGui::TableHeadersRow();

        for (int row = 0; row < 16; row++) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TableSetBgColor(
                ImGuiTableBgTarget_CellBg,
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]), 0
            );

            ImGui::Text("%02X", row << 4);

            for (int column = 0; column < 16; column++) {
                ImGui::TableSetColumnIndex(column+1);
                                        
                auto instr = Disassembler::getInstruction(row * 16 + column);
                
                ImGui::Selectable((std::string(instr.mnemonic) + " " + std::string(instr.operand)).c_str());
                ImGui::SetItemTooltip("%02X", (row << 4) | column);
            }
        }
        ImGui::EndTable();
    }
};

#endif // UI_CMD_TABLE_HPP
