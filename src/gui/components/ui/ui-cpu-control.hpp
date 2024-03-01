#include "../irenderable.hpp"

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
    std::function<void(UiCpuControlRegister, uint8_t)> onRegisterChange;
    std::function<void(UiCpuControlFlags, bool)> onFlagChange;
    std::function<void(const char*)> onControlButton;
};

class UiCpuControl : public IRenderable {
public: 
    UiCpuControl(UiCpuControlEmits emits) : m_emits(emits) {}

    void render() override {
        renderControls();
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

    const char* m_controlButtons[5] = {
        "Start", "CMD+", "CMD++", "Stop", "Reset"
    };

    void renderControls() {
        for (int i = 0; i < 5; i++) {
            if (ImGui::Button(m_controlButtons[i]))
                m_emits.onControlButton(m_controlButtons[i]);
            
            ImGui::SameLine();
        }   
    }

    void renderRegisters() {
        if (!ImGui::BeginTable("Registers", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            return;

        const char* registers[] = {
            "B", "C", "D", "E", "H", "L", "M", "A"
        };

        for (auto r : registers) {
            ImGui::TableSetupColumn(r);
        }
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        for (int c = 0; c < 8; c++) {
            ImGui::TableSetColumnIndex(c);

            ImGui::PushItemWidth(-1);
            if (ImGui::InputScalar(
                registers[c],     
                ImGuiDataType_U8, &m_registers[c],
                NULL, NULL, 
                "%02X"
            )) {
                m_emits.onRegisterChange((UiCpuControlRegister)c, m_registers[c]);
            }
        }

        ImGui::EndTable();  
    }

    void renderFlags() {
        if (!ImGui::BeginTable("Flags", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            return;

        ImGui::TableSetupColumn("Carry");
        ImGui::TableSetupColumn("Sign");
        ImGui::TableSetupColumn("Zero");
        ImGui::TableSetupColumn("AC");
        ImGui::TableSetupColumn("Parrity");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        for (int i = 0; i < 5; i++) {
            ImGui::TableSetColumnIndex(i);
            if (ImGui::Checkbox(std::to_string(i).c_str(), &m_flags[i])) {
                m_emits.onFlagChange((UiCpuControlFlags)i, m_flags[i]);
            }
        }        

        ImGui::EndTable();
    }

    void renderMisc() {
        if (!ImGui::BeginTable("Misc", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            return;

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