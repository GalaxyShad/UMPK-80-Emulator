#ifndef UI_CMD_TABLE_HPP
#define UI_CMD_TABLE_HPP

#include <imgui.h>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

#include "../../controller.hpp"

class UiCmdTable : public IRenderable {
public:
    UiCmdTable(Controller& controller) : m_controller(controller) {}

    void render() override {
        if (!ImGui::BeginTable(
            "UiCmdTable", 
            17, 
            ImGuiTableFlags_Borders | 
            ImGuiTableFlags_RowBg   | 
            ImGuiTableFlags_ScrollX | 
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_SizingFixedFit
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
                uint8_t cmd = (row << 4) | column;

                if (ImGui::Selectable((std::string(instr.mnemonic) + " " + std::string(instr.operand)).c_str())) {
                    m_controller.setCommand(cmd);
                }
                ImGui::SetItemTooltip("%02X", cmd);
            }
        }
        ImGui::EndTable();
    }
private:
    Controller& m_controller;
};

#endif // UI_CMD_TABLE_HPP
