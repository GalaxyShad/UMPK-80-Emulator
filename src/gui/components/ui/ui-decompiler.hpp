#ifndef UI_DECOMPILER_HPP
#define UI_DECOMPILER_HPP

#include <imgui.h>

#include "../irenderable.hpp"
#include "../../controller.hpp"

class UiDecompilerWindow : public IRenderable {
public:
    UiDecompilerWindow(Controller& controller) : m_controller(controller) {}

    void render() override {
        static char decompileFileName[255] = {0};
        static uint16_t fromAdr = 0x0000;
        static uint16_t len = 0x0100;

        ImGui::PushItemWidth(200);
        ImGui::InputText("File path", decompileFileName, 255);

        uint16_t min = 0x0000;
        uint16_t max = 0x0FFF;
        ImGui::DragScalar("Start ADR", ImGuiDataType_U16, &fromAdr, 1, &min,
                          &max, "%04x");

        ImGui::DragScalar("Length", ImGuiDataType_U16, &len, 1, &min, &max,
                          "%04x");

        if (ImGui::Button("Decompile")) {
            m_controller.decompileToFile(std::string(decompileFileName),
                                         fromAdr, len);
            ImGui::OpenPopup("Success");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Success", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Docompiled succesfully");
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
};

#endif // UI_DECOMPILER_HPP
