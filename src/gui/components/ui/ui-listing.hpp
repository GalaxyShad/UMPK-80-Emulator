#ifndef UI_LISTING_HPP
#define UI_LISTING_HPP

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <fstream>
#include <imgui.h>
#include <ostream>
#include <string>
#include <vector>

#include "../../controller.hpp"
#include "../irenderable.hpp"

struct UiListingLine {
    uint16_t address;
    uint8_t byte;
    std::string instruction;
};

class UiListing : public IRenderable {
public:
    UiListing(std::vector<UiListingLine>& listing, bool enableScroll = false, int* cursorPos = nullptr, int* breakpoint = nullptr ) 
        : m_listing(listing)
        , m_enableScroll(enableScroll)
        , m_cursorPos(cursorPos)
        , m_breakPoint(breakpoint)
    {}

    void render() override {
        renderAsTable();
    }
    
    void exportToStream(std::ostream& stream) {
        for (int row = 0; row < m_listing.size(); row++) {
            auto listingRow = m_listing[row];

            char str[255];
            std::snprintf(str, 255, "%04X | %02X | %s", 
                listingRow.address,
                listingRow.byte,
                listingRow.instruction.c_str());

            stream << std::string(str) << "\n";
        }
    }

private:
    std::vector<UiListingLine>& m_listing;
    bool m_enableScroll;
    int* m_cursorPos;
    int* m_breakPoint;

    char m_exportFileNameBuffer[255] = "disassembled.txt";
    int prevCursorPos = -1;

private:
    void renderAsTable() {
        if (ImGui::BeginTable("Dump", 4,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_ScrollX |
                                  ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedFit)
        ) {
            ImGui::TableSetupColumn("");
            ImGui::TableSetupColumn("ADR");
            ImGui::TableSetupColumn("MC");
            ImGui::TableSetupColumn("Mnemonic");

            ImGui::TableHeadersRow();

            ImGuiStyle &style = ImGui::GetStyle();

            for (int row = 0; row < m_listing.size(); row++) {
                auto listingRow = m_listing[row];
                auto color = (m_cursorPos && ((*m_cursorPos) == row))
                             ? style.Colors[ImGuiCol_ButtonHovered]
                             : style.Colors[ImGuiCol_Text];

                ImGui::TableNextRow();

                ImGui::TableSetBgColor(
                    ImGuiTableBgTarget_CellBg,
                    ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]),
                    0);

                if (m_breakPoint != nullptr) {
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(("##rb" + std::to_string(row)).c_str(), *m_breakPoint == row)) {
                        *m_breakPoint = (*m_breakPoint != row) ? row : -1;
                    }
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(color, "%04X", listingRow.address);

                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(color, "%02X", listingRow.byte);

                ImGui::TableSetColumnIndex(3);
                ImGui::TextColored(color, "%s", listingRow.instruction.c_str());

                if (m_enableScroll && m_cursorPos != nullptr &&
                    prevCursorPos != (*m_cursorPos) &&
                    (*m_cursorPos) == row
                ) {
                    ImGui::SetScrollHereY(0.25f);
                    prevCursorPos = (*m_cursorPos);
                }
            }

            ImGui::EndTable();
        }

        ImGui::Separator();

        ImGui::InputText("File name", m_exportFileNameBuffer, 255);

        if (ImGui::Button("Export")) {
            std::ofstream ofs(m_exportFileNameBuffer);
            exportToStream(ofs);
            ofs.close();
        }

        ImGui::Spacing();
    }
};

class UiMemoryDisassembler : public IRenderable {
public:
    UiMemoryDisassembler(int* cursor = nullptr, int* breakpoint = nullptr) 
        : m_uiListing(m_listing, true, cursor, breakpoint)
    {}

    void render() override {
        m_uiListing.render();
    }

    void disassemble(const uint8_t* memory, size_t size, uint16_t labelStart = 0x0000) {
        m_listing.clear();
        
        Disassembler disasm(memory, size);

        Disassembler::Result dis;
        while ((dis = disasm.translate()).bytesCount != 0) {
            for (int i = 0; i < dis.bytesCount; i++) {
                m_listing.push_back(
                    UiListingLine {
                        (uint16_t)(labelStart + dis.address + i), 
                        dis.bytes[i],
                        i == 0 ? dis.instruction : "-"
                    }
                );
            }
        }
    }

private:
    std::vector<UiListingLine> m_listing;
    UiListing                  m_uiListing;
};

class UiOsListing : public IRenderable {
public:
    UiOsListing(Controller &controller)
        : m_controller(controller)
        , m_uiDisassembler(&m_cursorpos, &m_controller.breakpoint)
    {
        m_uiDisassembler.disassemble(m_controller.getRom(), m_controller.UMPK_ROM_SIZE);
    }

    void render() override {
        m_cursorpos = m_controller.umpk().getCpu().getProgramCounter();
        m_uiDisassembler.render();
    }

private:
    int          m_cursorpos;
    Controller&  m_controller;
    UiMemoryDisassembler m_uiDisassembler;
};

class UiStack : public IRenderable {
public:
    UiStack(Controller &controller)
        : m_controller(controller) {}

    void render() override {
        if (ImGui::BeginTable("Stack", 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_ScrollX |
                                  ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedFit)
        ) {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Value");

            ImGui::TableHeadersRow();

            ImGuiStyle &style = ImGui::GetStyle();

            const uint8_t* rom = m_controller.getRom();
            int stackPointer = m_controller.umpk().getCpu().getStackPointer();

            constexpr uint16_t stackHigh = 0x0BD0;
            constexpr uint16_t stackLow  = 0x0B00;

            for (int addr = stackHigh; addr >= stackLow; addr--) {
                int row = stackHigh - addr;
                auto color = (stackPointer == addr)
                             ? style.Colors[ImGuiCol_ButtonHovered]
                             : style.Colors[ImGuiCol_Text];

                ImGui::TableNextRow();

                ImGui::TableSetBgColor(
                    ImGuiTableBgTarget_CellBg,
                    ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]),
                    0);

                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(color, "%04X", addr);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(color, "%02X", rom[addr]);

                if (stackPointer == addr) {
                    ImGui::SameLine();
                    ImGui::TextColored(style.Colors[ImGuiCol_ButtonHovered], " <- SP");
                }
            }

            ImGui::EndTable();
        }

        ImGui::Separator();

        ImGui::Text("Stack Pointer:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%04X", m_controller.umpk().getCpu().getStackPointer());

        ImGui::Spacing();
    }

private:
    Controller& m_controller;
};

#endif // UI_LISTING_HPP
