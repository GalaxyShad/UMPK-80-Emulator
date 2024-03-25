#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

#include <map>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "sfml-8seg-display.hpp"

#include "controller.hpp"
#include "viewcontrol.hpp"

class View : public ViewControl {
    public:
        View() : _dataContext(*this) {
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

        void errorMessageBox(std::string errMsg) {
            
            // ImGui::OpenPopup("Error");
            _errMsg = errMsg;
        }

        ~View() {
            ImGui::SFML::Shutdown();
        }
    private:
        Controller          _dataContext;

        std::string _errMsg;

        bool _port5[8] = { false };

        sf::RenderWindow    _window;
        sf::Clock           _deltaClock;

        uint16_t            _inputValuePgCounter = 0x0000;

        bool chboxesKeyboard[26] = {false};

        Sfml8SegmentDigit   _display[6] = {
            { sf::Vector2f(48*0, 0) }, 
            { sf::Vector2f(48*1, 0) }, 
            { sf::Vector2f(48*2, 0) }, 
            { sf::Vector2f(48*3, 0) }, 
            { sf::Vector2f(48*5, 0) }, 
            { sf::Vector2f(48*6, 0) }, 
        };

        void _update() {
            ImGui::SFML::Update(_window, _deltaClock.restart());

            _windowCode();
            // _windowInPorts();
            // _windowOutPorts();
            _windowControls();
            _windowMemory();
            _windowCpuInfo();
            _windowKeyboard();
            _windowPort5();
            _windowTests();

            _windowCommandTable();

            ImGui::ShowDemoWindow();

            if (!_errMsg.empty())
                ImGui::OpenPopup("Error");  //_dataContext.onBtnReset();

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("%s", _errMsg.c_str());
                ImGui::Separator();

                if (ImGui::Button("OK", ImVec2(120, 0))) { 
                    ImGui::CloseCurrentPopup(); 
                    _errMsg = ""; 
                }

                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }

            ImGui::Begin("Display");
            sf::RenderTexture rt;
            rt.create(336, 64);

            for (int i = 0; i < 6; i++) 
                _display[i].draw(rt);
            rt.display();
         
            ImGui::SetNextItemWidth(-100);
            ImGui::Image(rt); 

            ImGui::End();

            _window.clear();
            ImGui::SFML::Render(_window);
            _window.display();

            for (int i = 0; i < 6; i++) {
                _display[i].setValue(_dataContext.getDisplayDigit(i));
            }
        }

        void _handleEvents() {
            sf::Event event;
            while (_window.pollEvent(event)) {
                ImGui::SFML::ProcessEvent(_window, event);

                if (event.type == sf::Event::Closed) {
                    _window.close();
                }

                if (event.type == sf::Event::KeyPressed ||
                    event.type == sf::Event::KeyReleased
                ) {
                    bool keyState = (event.type == sf::Event::KeyPressed);

                    std::map<sf::Keyboard::Key, Keyboard::Key> mapKeys{
                        { sf::Keyboard::Num0, Keyboard::Key::_0 },
                        { sf::Keyboard::Num1, Keyboard::Key::_1 },
                        { sf::Keyboard::Num2, Keyboard::Key::_2 },
                        { sf::Keyboard::Num3, Keyboard::Key::_3 },
                        { sf::Keyboard::Num4, Keyboard::Key::_4 },
                        { sf::Keyboard::Num5, Keyboard::Key::_5 },
                        { sf::Keyboard::Num6, Keyboard::Key::_6 },
                        { sf::Keyboard::Num7, Keyboard::Key::_7 },
                        { sf::Keyboard::Num8, Keyboard::Key::_8 },
                        { sf::Keyboard::Num9, Keyboard::Key::_9 },
                        { sf::Keyboard::A, Keyboard::Key::_A },
                        { sf::Keyboard::B, Keyboard::Key::_B },
                        { sf::Keyboard::C, Keyboard::Key::_C },

                        { sf::Keyboard::D, Keyboard::Key::_D },
                        { sf::Keyboard::E, Keyboard::Key::_E },
                        { sf::Keyboard::F, Keyboard::Key::_F },

                        { sf::Keyboard::Enter, Keyboard::Key::ZP_UV },
                        { sf::Keyboard::Space, Keyboard::Key::OT_A },

                        { sf::Keyboard::Right, Keyboard::Key::ZP_UV },
                        { sf::Keyboard::Left, Keyboard::Key::UM },
                        { sf::Keyboard::Up, Keyboard::Key::P },
                        { sf::Keyboard::Down, Keyboard::Key::ST },

                        { sf::Keyboard::F5, Keyboard::Key::P },
                    };

                    if (mapKeys.count(event.key.code)) {
                        _dataContext.onUmpkKeySet(mapKeys[event.key.code], keyState);
                    }
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
            if (ImGui::BeginTable(
                "ROM and RAM", 
                17, 
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)
            ) {
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableSetupColumn("");
                for (int column = 0; column < 16; column++)
                    ImGui::TableSetupColumn(("0" + std::string(1, "0123456789ABCDEF"[column])).c_str());
                ImGui::TableHeadersRow();

                for (int row = 0; row < 0x1000 / 16; row++) {
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

                        ImGui::Text(
                            "%02X", 
                            _dataContext.getUmpk().getBus().memoryRead(row*16+column)
                        );
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        void _windowCode() {
            ImGui::Begin("Code");

            ImGui::BeginChild(
                ImGui::GetID((void*)(intptr_t)0), 
                ImVec2(-1, -1), 
                true
            );

            Cpu& cpu = _dataContext.getUmpk().getCpu();
            Disassembler& dasm = _dataContext.getDisasm();
            dasm.reset();

            int cmdSleep = 1;
            for (int row = 0; row < 0x1000; row++) {
                static uint16_t prevPg = 0x0000;

                uint8_t cmd = _dataContext.getUmpk().getBus().memoryRead(row);

                std::string cmdName;
                if (cmdSleep == 1) {
                    cmdSleep = dasm.getInstructionBytes(cmd);
                    cmdName = std::string(dasm.translate());
                } else {
                    cmdName = " ";
                    cmdSleep--;
                }

                if (row == cpu.getProgramCounter()) {
                    ImGui::TextColored(
                        ImVec4(1, 1, 0, 1), 
                        "%04X | %02X | %s", 
                        row, cmd, cmdName.c_str() 
                    );

                    if (prevPg != cpu.getProgramCounter()) {
                        ImGui::SetScrollHereY(0.25f); 
                        prevPg = cpu.getProgramCounter();
                    }
                } else {
                    ImGui::Text(
                        "%04X | %02X | %s", 
                        row, cmd, cmdName.c_str() 
                    );
                }
            }


            
            ImGui::EndChild();

            // ImGui::PushItemWidth(-100);

            ImGui::End();
        }

        void _windowCpuInfoUnsafe() {
            ImGui::Begin("CPU Info (Unsafe)");

            Cpu& cpu = _dataContext.getUmpk().getCpu();

            // ImGui::BeginChild(
            //     ImGui::GetID((void*)(intptr_t)0), 
            //     ImVec2(320, 100), 
            //     true
            // );

            ImGui::Text("Registers");

            uint8_t regs[8] = {0};
            const char* labels[] = {
                "B", "C",
                "D", "E",
                "H", "L"
            };
            for (int c = 0; c < 6; c++) {
                    
                regs[c] = cpu.getRegister((Cpu::Register)c);

                ImGui::PushItemWidth(50);
                ImGui::InputScalar(
                    labels[c],     
                    ImGuiDataType_U8,  
                    &regs[c],
                    NULL, NULL, 
                    "%02X"
                );

                if (c % 2 == 0) ImGui::SameLine();
            }

            ImGui::Text("Flags");
            CpuFlagsMapping flags = cpu.getFlags();

            bool boolFlags[5] = {
                (bool)flags.carry,
                (bool)flags.sign,
                (bool)flags.zero,
                (bool)flags.auxcarry,
                (bool)flags.parry
            };

            const char* labelFlags[5] = {
                "Carry",
                "Sign",
                "Zero",
                "AC",
                "Parrity"
            };


            for (int i = 0; i < 5; i++) {
                ImGui::Checkbox(labelFlags[i], &boolFlags[i]);
                ImGui::SameLine();
            }

            // ImGui::EndChild();

            ImGui::End();



            // if (ImGui::BeginTable("Registers", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            //     ImGui::TableSetupColumn("B");
            //     ImGui::TableSetupColumn("C");
            //     ImGui::TableSetupColumn("D");
            //     ImGui::TableSetupColumn("E");
            //     ImGui::TableSetupColumn("H");
            //     ImGui::TableSetupColumn("L");
            //     ImGui::TableSetupColumn("M");
            //     ImGui::TableSetupColumn("A");
            //     ImGui::TableHeadersRow();

            //     ImGui::TableNextRow();

            //     uint8_t regs[8] = {0};

            //     for (int c = 0; c < 8; c++) {
            //         ImGui::TableSetColumnIndex(c);

            //         if (c != 6 || (c == 6 && cpu.getRegister(Cpu::Register::H) < 0x10))
            //             regs[c] = cpu.getRegister((Cpu::Register)c);


            //         ImGui::PushItemWidth(-1);
            //         ImGui::InputScalar(
            //             "a",     
            //             ImGuiDataType_U8,  
            //             &regs[c],
            //             NULL, NULL, 
            //             "%02x"
            //         );
            //         // ImGui::PushItemWidth();


            //         // if (c != 6 || (c == 6 && cpu.getRegister(Cpu::Register::H) < 0x10))
            //         //     ImGui::Text("%02x", cpu.getRegister((Cpu::Register)c));
            //         // else
            //         //     ImGui::Text("ERR");

            //     }
            //     ImGui::EndTable();
            // }

            // if (ImGui::BeginTable("Flags", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            //     ImGui::TableSetupColumn("Carry");
            //     ImGui::TableSetupColumn("Sign");
            //     ImGui::TableSetupColumn("Zero");
            //     ImGui::TableSetupColumn("AC");
            //     ImGui::TableSetupColumn("Parrity");
            //     ImGui::TableHeadersRow();

            //     ImGui::TableNextRow();

            //     CpuFlagsMapping flags = cpu.getFlags();

            //     ImGui::TableSetColumnIndex(0);
            //     ImGui::Text("%x", flags.carry);
            //     ImGui::TableSetColumnIndex(1);
            //     ImGui::Text("%x", flags.sign);
            //     ImGui::TableSetColumnIndex(2);
            //     ImGui::Text("%x", flags.zero);
            //     ImGui::TableSetColumnIndex(3);
            //     ImGui::Text("%x", flags.auxcarry);
            //     ImGui::TableSetColumnIndex(4);
            //     ImGui::Text("%x", flags.parry);

            //     ImGui::EndTable();
            // }

            // if (ImGui::BeginTable("Misc", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            //     ImGui::TableSetupColumn("Program Counter");
            //     ImGui::TableSetupColumn("Stack Pointer");
            //     ImGui::TableSetupColumn("Reg CMD");
            //     ImGui::TableHeadersRow();

            //     ImGui::TableNextRow();

            //     ImGui::TableSetColumnIndex(0);
            //     ImGui::Text("%04x", cpu.getProgramCounter());
            //     ImGui::TableSetColumnIndex(1);
            //     ImGui::Text("%04x", cpu.getStackPointer());
            //     ImGui::TableSetColumnIndex(2);
            //     ImGui::Text("%02x", cpu.getCommandRegister());
                
            //     ImGui::EndTable();
            // }

            // ImGui::InputScalar(
            //     "Manual Program Counter",     
            //     ImGuiDataType_U16,    
            //     &_inputValuePgCounter,
            //     NULL, NULL, 
            //     "%04x"
            // );

            // if (ImGui::Button("Set")) {
            //     // _dataContext.onSetProgramCounter(_inputValuePgCounter);
            // }

            // ImGui::End();
        }

        void _windowCpuInfo() {
            ImGui::Begin("CPU Info");

            Cpu& cpu = _dataContext.getUmpk().getCpu();

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

                uint8_t regs[8] = {0};

                for (int c = 0; c < 8; c++) {
                    ImGui::TableSetColumnIndex(c);

                    // if (c != 6 || (c == 6 && cpu.getRegister(Cpu::Register::H) < 0x10))
                    //     regs[c] = cpu.getRegister((Cpu::Register)c);


                    // ImGui::PushItemWidth(-1);
                    // ImGui::InputScalar(
                    //     "a",     
                    //     ImGuiDataType_U8,  
                    //     &regs[c],
                    //     NULL, NULL, 
                    //     "%02x"
                    // );
                    // // ImGui::PushItemWidth();


                    if (c != 6 || (c == 6 && cpu.getRegister(Cpu::Register::H) < 0x10))
                        ImGui::Text("%02X", cpu.getRegister((Cpu::Register)c));
                    else
                        ImGui::Text("ERR");

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

                CpuFlagsMapping flags = cpu.getFlags();

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
                ImGui::Text("%04x", cpu.getProgramCounter());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%04x", cpu.getStackPointer());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%02x", cpu.getCommandRegister());
                
                ImGui::EndTable();
            }

            ImGui::End();
        }

        void _windowKeyboard() {
            ImGui::Begin("Keyboard");

            ImVec2 btnSize = ImVec2(32, 32);

            ImGui::Button(" ", btnSize); ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::R, ImGui::Button("R", btnSize));        ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::SHC, ImGui::Button("SHC", btnSize));    ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_C, ImGui::Button("C", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_D, ImGui::Button("D", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_E, ImGui::Button("E", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_F, ImGui::Button("F", btnSize));

            ImGui::Button(" ", btnSize); ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::SHK, ImGui::Button("SHK", btnSize));         ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::PR_SCH, ImGui::Button("PrSch", btnSize));    ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_8, ImGui::Button("8", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_9, ImGui::Button("9", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_A, ImGui::Button("A", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_B, ImGui::Button("B", btnSize));

            _dataContext.onUmpkKeySet(Keyboard::Key::ST, ImGui::Button("St", btnSize));      ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::OT_RG, ImGui::Button("OtRg", btnSize)); ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::OT_A, ImGui::Button("OtA", btnSize));   ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_4, ImGui::Button("4", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_5, ImGui::Button("5", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_6, ImGui::Button("6", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_7, ImGui::Button("7", btnSize));       

            _dataContext.onUmpkKeySet(Keyboard::Key::P, ImGui::Button("P", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::UM, ImGui::Button("Um", btnSize));    ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::ZP_UV, ImGui::Button("ZpUv", btnSize));  ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_0, ImGui::Button("0", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_1, ImGui::Button("1", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_2, ImGui::Button("2", btnSize));       ImGui::SameLine();
            _dataContext.onUmpkKeySet(Keyboard::Key::_3, ImGui::Button("3", btnSize));      

            ImGui::End();
        }

        void _windowPort5() {
            ImGui::Begin("Port 05");


            bool leds[8];
            uint8_t inData = _dataContext.port5Out();
            for (int i = 0; i < 8; i++) {
                leds[7-i] = (inData >> i) & 0b1;
            }

            // ImGui::Text("Output");
            for (int i = 0; i < 8; i++) {
                ImGui::RadioButton(("o" + std::to_string(7-i)).c_str(), leds[i]);
                ImGui::SameLine();
            }


            ImGui::NewLine();
            ImGui::Separator();

            // ImGui::Text("Input");
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

            _dataContext.port5In(data);

            ImGui::End();
        }

        void _windowTests() {
            ImGui::Begin("Tests");

            static char pathFile[255] = {0};
            ImGui::PushItemWidth(200);
            ImGui::InputText("File path", pathFile, 255);
            ImGui::SameLine();
            
            static std::vector<uint8_t> program;
            static std::string err;

            if (ImGui::Button("Open", ImVec2(100, 0))) {
                program = _dataContext.readBinaryFile(std::string(pathFile));

                if (program.size() == 0)
                    err = "Cannot open a file " + std::string(pathFile);
            }

            if (!err.empty() && program.size() == 0)
                ImGui::TextColored(ImVec4(255, 0, 0, 127), err.c_str());

            if (program.size() != 0)
                ImGui::TextColored(
                    ImVec4(0, 255, 0, 127), 
                    ("File " + std::string(pathFile) + " is loaded succesfully").c_str()
                );

            if (program.size() != 0) {
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
                    if (ImGui::BeginTabItem("RAW")) {
                        if (ImGui::BeginTable(
                            "Loaded program", 17,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY,
                            ImVec2(0, 320))
                        ) {
                            ImGui::TableSetupScrollFreeze(1, 1);
                            ImGui::TableSetupColumn("");
                            for (int column = 0; column < 16; column++)
                                ImGui::TableSetupColumn(("0" + std::string(1, "0123456789ABCDEF"[column])).c_str());
                            ImGui::TableHeadersRow();

                            for (int row = 0; row < (program.size() / 16) + 1; row++) {
                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                ImGui::TableSetBgColor(
                                    ImGuiTableBgTarget_CellBg,
                                    ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]), 0
                                );

                                
                                ImGui::Text("%04X", row << 4);

                                for (int column = 0; column < 16; column++) {
                                    ImGui::TableSetColumnIndex(column+1);

                                    uint8_t val = (row*16+column < program.size()) ? program[row*16+column] : 0x00;
                                    ImGui::Text("%02X",  val);
                                }
                            }
                            ImGui::EndTable();
                        }
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Decompiled")) {
                        ImGui::BeginChild(
                            ImGui::GetID((void*)(intptr_t)0), 
                            ImVec2(-1, 320), 
                            true
                        );

                        Disassembler& dasm = _dataContext.getDisasm();

                        int cmdSleep = 1;
                        for (int row = 0; row < program.size(); row++) {
                            static uint16_t prevPg = 0x0000;

                            uint8_t cmd = program[row];

                            std::string cmdName;
                            if (cmdSleep == 1) {
                                cmdSleep = dasm.getInstructionBytes(cmd);
                                cmdName  = dasm.getInstruction(cmd);
                            } else {
                                cmdName = " ";
                                cmdSleep--;
                            }

                            ImGui::Text(
                                "%04X | %02X | %s", 
                                row, cmd, cmdName.c_str() 
                            );
                        }

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();

                static uint16_t copyTo = 0x800;

                uint16_t min = 0x0800;
                uint16_t max = 0x0FFF; 

                ImGui::Separator();
                ImGui::DragScalar(
                    "Memory position",
                    ImGuiDataType_U16,
                    &copyTo,
                    1,
                    &min,
                    &max,
                    "%04x"
                );

                ImGui::Separator();
                if (ImGui::Button("Load", ImVec2(100, 0))) {
                    _dataContext.loadProgramToMemory(copyTo, program);
                };
                
            }

            ImGui::End();
        }

        void _windowCommandTable() {
            ImGui::Begin("Command table");

            if (ImGui::BeginTable(
                "ROM and RAM", 
                17, 
                ImGuiTableFlags_Borders | 
                ImGuiTableFlags_RowBg | 
                ImGuiTableFlags_ScrollX | 
                ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_SizingFixedSame)
            ) {
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableSetupColumn("");

                const Keyboard::Key keys[16] = {
                    Keyboard::Key::_0,
                    Keyboard::Key::_1,
                    Keyboard::Key::_2,
                    Keyboard::Key::_3,
                    Keyboard::Key::_4,
                    Keyboard::Key::_5,
                    Keyboard::Key::_6,
                    Keyboard::Key::_7,
                    Keyboard::Key::_8,
                    Keyboard::Key::_9,
                    Keyboard::Key::_A,
                    Keyboard::Key::_B,
                    Keyboard::Key::_C,
                    Keyboard::Key::_D,
                    Keyboard::Key::_E,
                    Keyboard::Key::_F
                };

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
                                                
                        std::string instr = _dataContext.getDisasm().getInstruction(row * 16 + column);
                        
                        std::transform(instr.begin(), instr.end(), instr.begin(), ::toupper); 
                        ImGui::Selectable(instr.c_str());
                        // if (ImGui::Selectable(
                        //     instr.c_str()
                        // )) {
                        //     uint8_t cmdByte = row * 16 + column;
                        //     _dataContext.onUmpkKeySet(keys[cmdByte >> 4], true);
                        //     _dataContext.onUmpkKeySet(keys[cmdByte & 0xF], true);
                        // }

                    }
                }
                ImGui::EndTable();
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

