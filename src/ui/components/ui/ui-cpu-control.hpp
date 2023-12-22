#include "irenderable.hpp"

#include <string>
#include <functional>
#include <imgui.h>

enum UiCpuControlRegister {
    B, C, 
    D, E, 
    H, L, 
    A, M
};

enum UiCpuControlFlags {
    CARRY,
    SIGN,
    ZERO,
    AUX_CARRY,
    PARRY
};

struct UiCpuControlEmits {
    std::function<void(UiCpuControlRegister, uint8_t)>& emitRegisterChange;
    std::function<void(UiCpuControlFlags, bool)>& emitFlagChange;
};

class UiCpuControl : public IRenderable {
public: 
    UiCpuControl(UiCpuControlEmits emits) : m_emits(emits) {}

    void render() override {
        renderRegisters();
        renderFlags();
        renderMisc();
    }

    void setRegister(UiCpuControlRegister reg, uint8_t value) {
        m_registers[reg] = value;
    }

    void setFlag(UiCpuControlFlags flag, bool value) {
        m_flags[flag] = value;
    }

    void setPgCounter(uint16_t pgCounter) {
        m_programCounter = pgCounter;
    }

    void setStackPointer(uint16_t stackPointer) {
        m_stackPointer = stackPointer;
    }

    void setCmd(uint8_t cmd) {
        m_cmd = cmd;
    }

private:
    UiCpuControlEmits m_emits;
    uint16_t m_stackPointer = 0x0000;
    uint16_t m_programCounter = 0x0000;
    uint8_t m_cmd = 0x00;

    uint8_t m_registers[8] = { 0 };
    bool m_flags[5] = { 0 };


    void renderRegisters() {
        ImGui::BeginTable("Registers", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

        ImGui::TableSetupColumn("B");
        ImGui::TableSetupColumn("C");
        ImGui::TableSetupColumn("D");
        ImGui::TableSetupColumn("E");
        ImGui::TableSetupColumn("H");
        ImGui::TableSetupColumn("L");
        ImGui::TableSetupColumn("M");
        ImGui::TableSetupColumn("A");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        for (int c = 0; c < 8; c++) {
            ImGui::TableSetColumnIndex(c);

            ImGui::PushItemWidth(-1);
            if (ImGui::InputScalar(
                "a",     
                ImGuiDataType_U8, &m_registers[c],
                NULL, NULL, 
                "%02X"
            )) {
                m_emits.emitRegisterChange((UiCpuControlRegister)c, m_registers[c]);
            }
        }

        ImGui::EndTable();  
    }

    void renderFlags() {
        ImGui::BeginTable("Flags", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

        ImGui::TableSetupColumn("Carry");
        ImGui::TableSetupColumn("Sign");
        ImGui::TableSetupColumn("Zero");
        ImGui::TableSetupColumn("AC");
        ImGui::TableSetupColumn("Parrity");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        for (int i = 0; i < 5; i++) {
            ImGui::TableSetColumnIndex(i);
            ImGui::Checkbox(std::to_string(m_flags[i]).c_str(), &m_flags[i]);
        }        

        ImGui::EndTable();
    }

    void renderMisc() {
        ImGui::BeginTable("Misc", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);

        ImGui::TableSetupColumn("Program Counter");
        ImGui::TableSetupColumn("Stack Pointer");
        ImGui::TableSetupColumn("Reg CMD");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%04X", m_programCounter);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%04X", m_stackPointer);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%02X", m_cmd);
        
        ImGui::EndTable();
    }
};