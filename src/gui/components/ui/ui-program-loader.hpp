#ifndef UI_PROGRAM_LOADER_HPP
#define UI_PROGRAM_LOADER_HPP

#include <imgui.h>
#include "../irenderable.hpp"
#include "../../controller.hpp"
#include "ui-memory-dump.hpp"
#include "ui-listing.hpp"
#include "../../../core/disassembler.hpp"

class UiProgramLoader : public IRenderable {
public:
    UiProgramLoader(Controller& controller) 
        : m_controller(controller) 
        , m_uiRaw(m_program.data(), m_program.size())
        , m_disassembler(m_program.data(), m_program.size())
    {}

    void render() override {
        static char pathFile[255] = "main.i8080asm.bin";
        ImGui::PushItemWidth(200);
        ImGui::InputText("File path", pathFile, 255);
        ImGui::SameLine();

        if (ImGui::Button("Open", ImVec2(100, 0))) {
            try {
                m_program = m_controller.readBinaryFile(std::string(pathFile));

                if (m_program.size() == 0)
                    m_errorMessage = "Program size length = 0 bytes " + std::string(pathFile);

                m_uiRaw.update(m_program.data(), m_program.size());

                m_uiMemoryDisassembler.disassemble(m_program.data(), m_program.size());
            } catch (std::runtime_error err) {
                m_errorMessage = "Cannot open a file " + std::string(pathFile);
            }
        }

        if (!m_errorMessage.empty() && m_program.size() == 0)
            ImGui::TextColored(ImVec4(255, 0, 0, 127), "%s", m_errorMessage.c_str());

        if (m_program.size() != 0) {
            auto succesMessage = "File " + std::string(pathFile) + " is loaded succesfully";

            ImGui::TextColored(ImVec4(0, 255, 0, 127), "%s", succesMessage.c_str());

            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
                
                if (ImGui::BeginTabItem("RAW")) {
                    m_uiRaw.render();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Decompiled")) {
                    m_uiMemoryDisassembler.render();

                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();

            static uint16_t copyTo = 0x800;

            uint16_t min = 0x0800;
            uint16_t max = 0x0FFF;

            ImGui::Separator();
            ImGui::DragScalar("Memory position", ImGuiDataType_U16, &copyTo, 1,
                              &min, &max, "%04x");

            ImGui::Separator();
            if (ImGui::Button("Load", ImVec2(100, 0))) {
                m_controller.loadProgramToMemory(copyTo, m_program);
            };
        }
    }

private:
    Controller& m_controller;
    std::vector<uint8_t> m_program;

    std::string m_errorMessage;
    std::vector<UiListingLine> m_listing;

    UiMemoryDump m_uiRaw;
    UiMemoryDisassembler m_uiMemoryDisassembler;
    Disassembler m_disassembler;
};

#endif // UI_PROGRAM_LOADER_HPP
