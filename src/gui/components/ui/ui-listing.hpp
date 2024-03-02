#ifndef UI_LISTING_HPP
#define UI_LISTING_HPP

#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

struct UiListingLine {
    uint16_t address;
    std::vector<uint8_t> bytes;
    std::string instruction;
};

struct UiListingProps {
    std::vector<UiListingLine>& listing;
    bool enableScroll = false;
    int* cursorPos = nullptr;
};

class UiListing : public IRenderable {
public:
    UiListing(UiListingProps props) : m_props(props) {}

    void render() override {
        ImGui::BeginChild(
            ImGui::GetID((void*)(intptr_t)0), 
            ImVec2(-1, -1), 
            true
        );

        for (int row = 0; row < m_props.listing.size(); row++) {
            auto listingRow = m_props.listing[row];
            auto color = (m_props.cursorPos && ((*m_props.cursorPos) == row))
                ? ImVec4(1, 1, 0, 1) 
                : ImVec4(1, 1, 1, 1);

            ImGui::TextColored(
                color, 
                "%04X | %02X | %s", 
                listingRow.address, 
                0x00,
                // listingRow.bytes, 
                listingRow.instruction.c_str() 
            );

            if (m_props.enableScroll && m_props.cursorPos != nullptr 
                && prevCursorPos != (*m_props.cursorPos)
            ) {
                ImGui::SetScrollHereY(0.25f); 
                prevCursorPos = (*m_props.cursorPos);
            }
        }

        ImGui::EndChild();
    }

private:
    UiListingProps m_props;
    int prevCursorPos = -1;
};

#endif // UI_LISTING_HPP
