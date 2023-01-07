#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "sfml-8seg-display.hpp"

#include "controller.hpp"

class View {
    public:
        View() {
            _window.create(sf::VideoMode(1280, 720), "UMPK-80 Emulator");
            ImGui::SFML::Init(_window);
            _applyTheme();
        }

        void start() {
            while (_window.isOpen()) {
                _handleEvents();
                _update();
            }
        }

        ~View() {
            ImGui::SFML::Shutdown();
        }
    private:
        Controller          _dataContext;

        bool _port5[8] = { false };

        sf::RenderWindow    _window;
        sf::Clock           _deltaClock;

        uint16_t            _inputValuePgCounter = 0x0000;

        bool chboxesKeyboard[26] = {false};

        Sfml8SegmentDigit   _display[6] = {
            { sf::Vector2f(64+48*0, 32) }, 
            { sf::Vector2f(64+48*1, 32) }, 
            { sf::Vector2f(64+48*2, 32) }, 
            { sf::Vector2f(64+48*3, 32) }, 
            { sf::Vector2f(64+48*5, 32) }, 
            { sf::Vector2f(64+48*6, 32) }, 
        };

        void _update() {
            ImGui::SFML::Update(_window, _deltaClock.restart());

            _windowCode();
            _windowInPorts();
            _windowOutPorts();
            _windowControls();
            _windowMemory();
            _windowCpuInfo();
            _windowKeyboard();
            _windowPort5();
            _windowTests();

            // ImGui::ShowDemoWindow();

            _window.clear();

            for (int i = 0; i < 6; i++) 
                _display[i].draw(_window);
            
            ImGui::SFML::Render(_window);
            _window.display();

            for (int i = 0; i < 6; i++) {
                _display[i].setValue(_dataContext.getDisplay().get(i));
            }
        }

        void _handleEvents() {
            sf::Event event;
            while (_window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(_window, event);

                if (event.type == sf::Event::Closed) {
                    _window.close();
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////

        void _windowControls() {
            ImGui::Begin("Controls");

            if (ImGui::Button("Start"))     _dataContext.onBtnStart();
            ImGui::SameLine();

            if (ImGui::Button("CMD+"))      _dataContext.onBtnNextCommand();
            ImGui::SameLine();

            if (ImGui::Button("Stop"))      _dataContext.onButtonStop();
            ImGui::SameLine();

            if (ImGui::Button("Reset"))     _dataContext.onBtnReset();
            ImGui::End();
        }

        void _windowMemory() {
            ImGui::Begin("Memory");
            if (ImGui::BeginTable("ROM and RAM", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("");
                for (int column = 0; column < 16; column++)
                    ImGui::TableSetupColumn(std::to_string(column).c_str());
                ImGui::TableHeadersRow();

                for (int row = 0; row < 0xFFFF / 16; row++) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%04x", row << 4);

                    for (int column = 0; column < 16; column++) {
                        ImGui::TableSetColumnIndex(column+1);
                        ImGui::Text("%02x", _dataContext.getBus().read(row*16+column));
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        void _windowOutPorts() {
            ImGui::Begin("Out Ports");
            if (ImGui::BeginTable("Out Ports", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("");
                for (int column = 0; column < 16; column++)
                    ImGui::TableSetupColumn(std::to_string(column).c_str());
                ImGui::TableHeadersRow();

                for (int row = 0; row < 256 / 16; row++) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%02x", row << 4);

                    for (int column = 0; column < 16; column++) {
                        ImGui::TableSetColumnIndex(column+1);
                        ImGui::Text("%02x", _dataContext.getBus().getOutPortData(row*16+column));
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        void _windowInPorts() {
            ImGui::Begin("In Ports");
            if (ImGui::BeginTable("In Ports", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("");
                for (int column = 0; column < 16; column++)
                    ImGui::TableSetupColumn(std::to_string(column).c_str());
                ImGui::TableHeadersRow();

                for (int row = 0; row < 256 / 16; row++) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%02x", row << 4);

                    for (int column = 0; column < 16; column++) {
                        ImGui::TableSetColumnIndex(column+1);
                        ImGui::Text("%02x", _dataContext.getBus().portRead(row*16+column));
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        void _windowCode() {
            ImGui::Begin("Code");
            if (ImGui::BeginTable("Code", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("ADR");
                ImGui::TableSetupColumn("BYTE");
                ImGui::TableSetupColumn("ASM");
                ImGui::TableHeadersRow();

                _dataContext.getDisassembler().reset();
                int cmdSleep = 1;
                for (int row = 0; row < 0x0d00; row++) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%04x", row);

                    if (_dataContext.getCpu().getProgramCounter() == row)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 0xFF0A5F38);

                    ImGui::TableSetColumnIndex(1);

                    if (_dataContext.getCpu().getProgramCounter() == row)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 0xFF0A5F38);

                    uint8_t cmd = _dataContext.getBus().read(row);
                    ImGui::Text("%02x", cmd);

                    ImGui::TableSetColumnIndex(2);

                    if (cmdSleep == 1) {
                        cmdSleep = _dataContext.getDisassembler().getInstructionBytes(cmd);
                        ImGui::Text(_dataContext.getDisassembler().translate().c_str());
                    } else {
                        ImGui::Text("");
                        cmdSleep--;
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        void _windowCpuInfo() {
            ImGui::Begin("CPU Info");

            if (ImGui::BeginTable("Registers", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
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
                    ImGui::Text("%02x", _dataContext.getCpu().getRegister((Cpu::Register)c));
                }
                ImGui::EndTable();
            }

            if (ImGui::BeginTable("Flags", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Carry");
                ImGui::TableSetupColumn("Sign");
                ImGui::TableSetupColumn("Zero");
                ImGui::TableSetupColumn("AC");
                ImGui::TableSetupColumn("Parrity");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();

                CpuFlagsMapping flags = _dataContext.getCpu().getFlags();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%x", flags.carry);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%x", flags.sign);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%x", flags.zero);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%x", flags.auxcarry);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%x", flags.parry);

                ImGui::EndTable();
            }

            if (ImGui::BeginTable("Misc", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Program Counter");
                ImGui::TableSetupColumn("Stack Pointer");
                ImGui::TableSetupColumn("Reg CMD");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%04x", _dataContext.getCpu().getProgramCounter());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%04x", _dataContext.getCpu().getStackPointer());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%02x", _dataContext.getCpu().getCommandRegister());
                
                ImGui::EndTable();
            }

            ImGui::InputScalar(
                "Manual Program Counter",     
                ImGuiDataType_U16,    
                &_inputValuePgCounter,
                NULL, NULL, 
                "%04x"
            );

            if (ImGui::Button("Set")) {
                _dataContext.onSetProgramCounter(_inputValuePgCounter);
            }

            ImGui::End();
        }

        void _windowKeyboard() {
            ImGui::Begin("Keyboard");

            ImVec2 btnSize = ImVec2(32, 32);

            ImGui::Button(" ", btnSize); ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::R, ImGui::Button("R", btnSize));        ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::SHC, ImGui::Button("SHC", btnSize));    ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_C, ImGui::Button("C", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_D, ImGui::Button("D", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_E, ImGui::Button("E", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_F, ImGui::Button("F", btnSize));

            ImGui::Button(" ", btnSize); ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::SHK, ImGui::Button("PrSch", btnSize));  ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::SHC, ImGui::Button("SHC", btnSize));    ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_8, ImGui::Button("8", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_9, ImGui::Button("9", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_A, ImGui::Button("A", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_B, ImGui::Button("B", btnSize));

            _dataContext.getKeyboard().setKey(Keyboard::Key::ST, ImGui::Button("St", btnSize));      ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::OT_RG, ImGui::Button("OtRg", btnSize)); ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::OT_A, ImGui::Button("OtA", btnSize));   ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_4, ImGui::Button("4", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_5, ImGui::Button("5", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_6, ImGui::Button("6", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_7, ImGui::Button("7", btnSize));       

            _dataContext.getKeyboard().setKey(Keyboard::Key::P, ImGui::Button("P", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::UM, ImGui::Button("Um", btnSize));    ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::ZP_UV, ImGui::Button("ZpUv", btnSize));  ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_0, ImGui::Button("0", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_1, ImGui::Button("1", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_2, ImGui::Button("2", btnSize));       ImGui::SameLine();
            _dataContext.getKeyboard().setKey(Keyboard::Key::_3, ImGui::Button("3", btnSize));      

            ImGui::End();
        }

        void _windowPort5() {
            ImGui::Begin("Port 05");


            bool leds[8];
            uint8_t inData = _dataContext.getBus().getOutPortData(0x05);
            for (int i = 0; i < 8; i++) {
                leds[7-i] = (inData >> i) & 0b1;
            }

            for (int i = 0; i < 8; i++) {
                ImGui::Checkbox(("o" + std::to_string(7-i)).c_str(), &leds[i]);
                ImGui::SameLine();
            }

            ImGui::NewLine();
            
            for (int i = 0; i < 8; i++) {
                ImGui::Checkbox(("i" + std::to_string(7-i)).c_str(), &_port5[i]);
                ImGui::SameLine();
            }

            uint8_t data = 0x00;
            uint8_t temp = 0x00;
            for (int i = 0; i < 8; i++) {
                temp = _port5[i];
                data |= temp << (8 - i - 1);
            }

            _dataContext.getBus().portInWrite(0x05, data);

            ImGui::End();
        }

        void _windowTests() {
            ImGui::Begin("Tests");

            for (int i = 0; i < 6; i++) {
                if (ImGui::Button(("EVM Lab #" + std::to_string(i+1)).c_str())) {
                    _dataContext.loadTest(i+1);
                }
            }

            ImGui::End();
        }

        void _applyTheme() {
            ImGuiStyle& style = ImGui::GetStyle();

            // style.WindowPadding = ImVec2(15, 15);
            style.WindowRounding = 8.0;
            style.FramePadding = ImVec2(5, 5);
            style.ItemSpacing = ImVec2(12, 8);
            style.ItemInnerSpacing = ImVec2(8, 6);
            style.IndentSpacing = 25.0;
            style.ScrollbarSize = 15.0;
            style.ScrollbarRounding = 15.0;
            style.GrabMinSize = 15.0;
            style.GrabRounding = 7.0;
            style.ChildRounding = 8.0;
            style.FrameRounding = 6.0;

            style.Colors[ImGuiCol_Text] = ImVec4(0.95, 0.96, 0.98, 1.00);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.36, 0.42, 0.47, 1.00);
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11, 0.15, 0.17, 1.00);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15, 0.18, 0.22, 1.00);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08, 0.08, 0.08, 0.94);
            style.Colors[ImGuiCol_Border] = ImVec4(0.43, 0.43, 0.50, 0.50);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00, 0.00, 0.00, 0.00);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20, 0.25, 0.29, 1.00);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12, 0.20, 0.28, 1.00);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09, 0.12, 0.14, 1.00);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09, 0.12, 0.14, 0.65);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00, 0.00, 0.00, 0.51);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08, 0.10, 0.12, 1.00);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15, 0.18, 0.22, 1.00);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02, 0.02, 0.02, 0.39);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20, 0.25, 0.29, 1.00);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18, 0.22, 0.25, 1.00);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09, 0.21, 0.31, 1.00);
            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.28, 0.56, 1.00, 1.00);
            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28, 0.56, 1.00, 1.00);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37, 0.61, 1.00, 1.00);
            style.Colors[ImGuiCol_Button] = ImVec4(0.20, 0.25, 0.29, 1.00);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28, 0.56, 1.00, 1.00);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06, 0.53, 0.98, 1.00);
            style.Colors[ImGuiCol_Header] = ImVec4(0.20, 0.25, 0.29, 0.55);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26, 0.59, 0.98, 0.80);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26, 0.59, 0.98, 1.00);
            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26, 0.59, 0.98, 0.25);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26, 0.59, 0.98, 0.67);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06, 0.05, 0.07, 1.00);
            // style.Colors[ImGuiCol_But] = ImVec4(0.40, 0.39, 0.38, 0.16);
            // style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40, 0.39, 0.38, 0.39);
            // style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40, 0.39, 0.38, 1.00);
            style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61, 0.61, 0.61, 1.00);
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00, 0.43, 0.35, 1.00);
            style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90, 0.70, 0.00, 1.00);
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00, 0.60, 0.00, 1.00);
            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25, 1.00, 0.00, 0.43);
            // style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00, 0.98, 0.95, 0.73);
        }
};

