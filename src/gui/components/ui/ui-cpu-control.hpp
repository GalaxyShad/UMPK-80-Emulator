#ifndef UI_CPU_CONTROL_HPP
#define UI_CPU_CONTROL_HPP

#include "../irenderable.hpp"

#include <string>
#include <functional>
#include <imgui.h>

#include "../../controller.hpp"

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
    UiCpuControl(UiCpuControlEmits emits, Controller& controller) : m_emits(emits), m_controller(controller) {}

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
    Controller& m_controller;

    uint16_t m_stackPointer   = 0x0000;
    uint16_t m_programCounter = 0x0000;
    uint8_t  m_cmd = 0x00;
    
    uint8_t m_registers[8] = { 0 };
    bool m_flags[5] = { 0 };

    const char* m_controlButtons[5] = {
        "Start", "CMD+", "CMD++", "Stop", "Reset"
    };

    void renderControls() {
        for (int i = 0; i < 5; i++) {
            if (ImGui::Button(m_controlButtons[i])) {
                switch (i) {
                    case 0: m_controller.onBtnStart(); break;
                    case 1: m_controller.onBtnNextCommand(); break;
                    case 2:  break;
                    case 3: m_controller.onButtonStop(); break;
                    case 4: m_controller.onBtnReset(); break;
                }

                m_emits.onControlButton(m_controlButtons[i]);
            }
            
            ImGui::SameLine();
        }   
    }

    void renderRegisters() {
        if (!ImGui::BeginTable("Registers", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            return;

        const char* registerLabels[] = {
            "B", "C", "D", "E", "H", "L", "M", "A"
        };

        for (auto r : registerLabels) {
            ImGui::TableSetupColumn(r);
        }
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        auto& cpu = m_controller.umpk().getCpu();

        for (int i = 0; i < 8; i++)
            m_registers[i] = m_controller.getRegister((Cpu::Register)i);

        for (int c = 0; c < 8; c++) {
            ImGui::TableSetColumnIndex(c);

            ImGui::PushItemWidth(-1);

            if (m_controller.isUmpkRunning()) {
                ImGui::Text("%02X", m_registers[c]);
            } else {
                ImGui::BeginDisabled(c == 6);
                if (ImGui::InputScalar(
                    registerLabels[c],     
                    ImGuiDataType_U8, &m_registers[c],
                    NULL, NULL, 
                    "%02X"
                )) {
                    m_controller.setRegister((Cpu::Register)c, m_registers[c]);
                    m_emits.onRegisterChange((UiCpuControlRegister)c, m_registers[c]);
                }
                ImGui::EndDisabled();
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

        auto& cpu = m_controller.umpk().getCpu();
        auto flags = cpu.getFlags();

        // TODO Refactor me 
        m_flags[0] = flags.carry;
        m_flags[1] = flags.sign;
        m_flags[2] = flags.zero;
        m_flags[3] = flags.auxcarry;
        m_flags[4] = flags.parry;

        if (m_controller.isUmpkRunning()) {
            for (int i = 0; i < 5; i++) {
                ImGui::TableSetColumnIndex(i);
                ImGui::Text("%s", m_flags[i] ? "ACTIVE" : "-");
            }        
        } else {
             for (int i = 0; i < 5; i++) {
                ImGui::TableSetColumnIndex(i);
                ImGui::PushItemWidth(-1);
                if (ImGui::Checkbox("", &m_flags[i])) {
                    flags.carry = m_flags[0];
                    flags.sign = m_flags[1];
                    flags.zero = m_flags[2];
                    flags.auxcarry = m_flags[3];
                    flags.parry = m_flags[4];

                    cpu.setFlags(flags);
                }
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

        auto& cpu = m_controller.umpk().getCpu();

        m_programCounter = cpu.getProgramCounter();

        ImGui::TableSetColumnIndex(0);
        if (m_controller.isUmpkRunning()) {
            ImGui::Text("%04X", m_programCounter);
        } else {
            ImGui::PushItemWidth(-1);
            if (ImGui::InputScalar(
                "PC",     
                ImGuiDataType_U16, &m_programCounter,
                NULL, NULL, 
                "%04X"
            )) {
                m_controller.setCpuProgramCounter(m_programCounter);
            }
        }
        ImGui::TableSetColumnIndex(1);
        if (m_controller.isUmpkRunning()) {
            ImGui::Text("%04X", m_stackPointer);
        } else {
            ImGui::PushItemWidth(-1);
            if (ImGui::InputScalar(
                "SP",     
                ImGuiDataType_U16, &m_stackPointer,
                NULL, NULL, 
                "%04X"
            )) {
                m_controller.setCpuStackPointer(m_stackPointer);
            }
        }
        ImGui::TableSetColumnIndex(2);
        std::string mnemonic =
            Disassembler::getInstruction(cpu.getCommandRegister()).mnemonic;
        ImGui::Text("%02X | %s", cpu.getCommandRegister(), mnemonic.c_str());
        
        ImGui::EndTable();
    }
};

#endif // UI_CPU_CONTROL_HPP
