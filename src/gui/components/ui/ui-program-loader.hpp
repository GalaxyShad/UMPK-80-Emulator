#include <imgui.h>
#include "../irenderable.hpp"
#include "../../../controller.hpp"
#include "ui-memory-dump.hpp"
#include "ui-listing.hpp"
#include "../../../disassembler.hpp"

class UiProgramLoader : IRenderable {
public:
    UiProgramLoader(Controller& controller) 
        : m_controller(controller) 
        , m_raw(m_program)
        , m_decompiled(UiListingProps {m_listing, false, nullptr})
        , m_disassembler(m_program)
    {}

    void render() override {
        static char pathFile[255] = {0};
        ImGui::PushItemWidth(200);
        ImGui::InputText("File path", pathFile, 255);
        ImGui::SameLine();

        if (ImGui::Button("Open", ImVec2(100, 0))) {
            m_program = m_controller.readBinaryFile(std::string(pathFile));

            if (m_program.size() == 0)
                m_errorMessage = "Cannot open a file " + std::string(pathFile);
            else 
                makeLisitng();
        }

        if (!m_errorMessage.empty() && m_program.size() == 0)
            ImGui::TextColored(ImVec4(255, 0, 0, 127), "%s", m_errorMessage.c_str());

        if (m_program.size() != 0) {
            auto succesMessage = "File " + std::string(pathFile) + " is loaded succesfully";

            ImGui::TextColored(ImVec4(0, 255, 0, 127), "%s", succesMessage.c_str());
        } else {
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
                
                if (ImGui::BeginTabItem("RAW")) {
                    m_raw.render();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Decompiled")) {
                    m_decompiled.render();

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

    UiMemoryDump m_raw;
    UiListing m_decompiled;
    Disassembler m_disassembler;

private:

    void makeLisitng() {
        m_disassembler.reset();
        m_listing.clear();

        uint16_t prg = 0;
        std::string instr;
        while ((instr = m_disassembler.translateOld()) != "EOF") {
            int bytesCount = m_disassembler.getPgCounter() - prg;
            prg = m_disassembler.getPgCounter();

            std::vector<uint8_t> bytes;
            for (int i = 0; i < bytesCount; i++) {
                bytes.push_back(m_program[prg+i]);
            }

            m_listing.push_back(UiListingLine {m_disassembler.getPgCounter(), bytes, instr});
        }
    }
};