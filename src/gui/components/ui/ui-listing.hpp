#ifndef UI_LISTING_HPP
#define UI_LISTING_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#include "../../controller.hpp"
#include "../irenderable.hpp"

struct UiListingLine {
    uint16_t address;
    std::vector<uint8_t> bytes;
    std::string instruction;
};

struct UiListingProps {
    std::vector<UiListingLine> &listing;
    bool enableScroll = false;
    int *cursorPos = nullptr;
};

class UiListing : public IRenderable {
public:
    UiListing(UiListingProps props) : m_props(props) {}

    void render() override {
        ImGui::BeginChild(ImGui::GetID((void *)(intptr_t)0), ImVec2(-1, -1),
                          true);

        for (int row = 0; row < m_props.listing.size(); row++) {
            auto listingRow = m_props.listing[row];
            auto color = (m_props.cursorPos && ((*m_props.cursorPos) == row))
                             ? ImVec4(1, 1, 0, 1)
                             : ImVec4(1, 1, 1, 1);


            ImGui::TextColored(color, "%04X | %s | %s",
                                listingRow.address, vectorToHex(listingRow.bytes).c_str(),
                                // listingRow.bytes,
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

private:
    UiListingProps m_props;
    int prevCursorPos = -1;

private:
    std::string vectorToHex(const std::vector<uint8_t> &data) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::uppercase;

        ss << std::setw(2) << (int)data[0] << " ";

        if (data.size() > 1) {
            ss << std::setw(2) << (int)data[1];
        } else {
            ss << "  ";
        }

        if (data.size() > 2) {
            ss << std::setw(2) << (int)data[2];
        } else {
            ss << "  ";
        }

        return ss.str();
    }
};

class UiListingDecompiler : public IRenderable {
public:
    UiListingDecompiler(Controller &controller)
        : m_clisting(UiListingProps{m_listing, true, &m_cursorpos}),
          m_controller(controller) {}

    void render() override {
        m_listing.clear();

        m_cursorpos = m_controller.umpk().getCpu().getProgramCounter();

        disasm.reset();
        Disassembler::Result dis;

        while ((dis = disasm.translate()).bytesCount != 0) {
            for (int i = 0; i < dis.bytesCount; i++) {
                std::vector<uint8_t> bytes;
                bytes.push_back(dis.bytes[i]);
                m_listing.push_back(
                    UiListingLine{(uint16_t)(dis.address+i), bytes, i == 0 ? dis.instruction : "-"});
            }
        }

        m_clisting.render();
    }

private:
    int m_cursorpos;
    std::vector<UiListingLine> m_listing;
    UiListing m_clisting;
    Controller &m_controller;
    Disassembler disasm = Disassembler(m_controller.getRom(), 0x800);
};

#endif // UI_LISTING_HPP
