#ifndef UI_LISTING_HPP
#define UI_LISTING_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <string>
#include <vector>

#include "../../controller.hpp"
#include "../irenderable.hpp"

struct UiListingLine {
    uint16_t address;
    uint8_t byte;
    std::string instruction;
};

struct UiListingProps {
    std::vector<UiListingLine> &listing;
    bool enableScroll;
    int *cursorPos;
};

class UiListing : public IRenderable {
public:
    UiListing(UiListingProps props) : m_props(props) {}

    void render() override {
        // renderAsChild();
        renderAsTable();
    }



private:
    UiListingProps m_props;
    int prevCursorPos = -1;

private:
    void renderAsChild() {
        ImGui::BeginChild("##listing", ImVec2(-1, -1),
                          true);

        ImGuiStyle &style = ImGui::GetStyle();

        for (int row = 0; row < m_props.listing.size(); row++) {
            auto listingRow = m_props.listing[row];
            auto color = (m_props.cursorPos && ((*m_props.cursorPos) == row))
                             ? style.Colors[ImGuiCol_ButtonActive]
                             : style.Colors[ImGuiCol_Text];

            ImGui::TextColored(
                color, "%04X | %02X | %s", 
                
                listingRow.address,
                listingRow.byte,
                listingRow.instruction.c_str());

            if (m_props.enableScroll && m_props.cursorPos != nullptr &&
                prevCursorPos != (*m_props.cursorPos) &&
                (*m_props.cursorPos) == row) {
                ImGui::SetScrollHereY(0.25f);
                prevCursorPos = (*m_props.cursorPos);
            }
        }

        ImGui::EndChild();
    }

    void renderAsTable() {
        if (ImGui::BeginTable("Dump", 3,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_ScrollX |
                                  ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingFixedFit)
        ) {
            ImGui::TableSetupColumn("ADR");
            ImGui::TableSetupColumn("MC");
            ImGui::TableSetupColumn("Mnemonic");

            ImGui::TableHeadersRow();

            ImGuiStyle &style = ImGui::GetStyle();

            for (int row = 0; row < m_props.listing.size(); row++) {
                auto listingRow = m_props.listing[row];
                auto color = (m_props.cursorPos && ((*m_props.cursorPos) == row))
                             ? style.Colors[ImGuiCol_ButtonHovered]
                             : style.Colors[ImGuiCol_Text];

                ImGui::TableNextRow();

                ImGui::TableSetBgColor(
                    ImGuiTableBgTarget_CellBg,
                    ImColor(ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]),
                    0);

                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(color, "%04X", listingRow.address);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(color, "%02X", listingRow.byte);

                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(color, "%s", listingRow.instruction.c_str());

                if (m_props.enableScroll && m_props.cursorPos != nullptr &&
                    prevCursorPos != (*m_props.cursorPos) &&
                    (*m_props.cursorPos) == row
                ) {
                    ImGui::SetScrollHereY(0.25f);
                    prevCursorPos = (*m_props.cursorPos);
                }
            }

            ImGui::EndTable();
        }
    }
};

class UiMemoryDisassembler : public IRenderable {
public:
    UiMemoryDisassembler(int* cursor = nullptr) 
        : m_uiListing(UiListingProps{ m_listing, true, cursor })
    {}

    void render() override {
        m_uiListing.render();
    }

    void disassemble(const uint8_t* memory, size_t size) {
        Disassembler disasm(memory, size);

        Disassembler::Result dis;
        while ((dis = disasm.translate()).bytesCount != 0) {
            for (int i = 0; i < dis.bytesCount; i++) {
                m_listing.push_back(
                    UiListingLine {
                        (uint16_t)(dis.address + i), 
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
        , m_uiDisassembler(&m_cursorpos)
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

#endif // UI_LISTING_HPP
