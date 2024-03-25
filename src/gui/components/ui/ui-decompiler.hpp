#ifndef UI_DECOMPILER_HPP
#define UI_DECOMPILER_HPP

#include <imgui.h>

#include "../irenderable.hpp"
#include "../../controller.hpp"
#include "ui-listing.hpp"

class UiDecompilerWindow : public IRenderable {
public:
    UiDecompilerWindow(Controller& controller) : m_controller(controller) {}

    void render() override {
        static uint16_t fromAdr = 0x0000;
        static uint16_t len = 0x0100;


        ImGui::InputScalar("Start ADR", ImGuiDataType_U16, &fromAdr, 0, 0, "%04X");
        ImGui::InputScalar("Length", ImGuiDataType_U16, &len, 0, 0, "%04X");

        // TODO interval validation
        if (ImGui::Button("Disassemble")) {
            m_uiMemoryDisassembler.disassemble(m_controller.getRom(), len, fromAdr);
            ImGui::OpenPopup("Success");
        }

        m_uiMemoryDisassembler.render();

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Success", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Disassembled succesfully");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }
    }

private:
    Controller& m_controller;
    UiMemoryDisassembler m_uiMemoryDisassembler;
};

#endif // UI_DECOMPILER_HPP
