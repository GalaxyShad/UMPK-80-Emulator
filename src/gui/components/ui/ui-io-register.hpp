#ifndef UI_IO_REGISTER_HPP
#define UI_IO_REGISTER_HPP

#include "../irenderable.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <imgui.h>
#include <string>

#include "../../controller.hpp"

class UiIoRegister : public IRenderable {
public:
    UiIoRegister(Controller &controller) : m_controller(controller) {}

    void render() override {
        ImGui::Text("P%02X Out", m_controller.umpk().PORT_IO);

        for (uint8_t i = 0x80, c = 7; i != 0; i >>= 1, c--) {
            ImGui::RadioButton(("##o" + std::to_string(c)).c_str(),
                               m_controller.port5Out() & i);
            ImGui::SameLine();
        }

        ImGui::Text("= %02X", m_controller.port5Out());
        ImGui::SameLine();

        ImGui::NewLine();
        ImGui::Separator();

        ImGui::Text("P%02x In", m_controller.umpk().PORT_IO);

        uint8_t inData = vecToUint8(vecIn);
        for (int i = 0; i < 8; i++) {
            if (ImGui::Checkbox(("##i" + std::to_string(7 - i)).c_str(),
                                &vecIn[i])) {
                m_controller.port5In(inData);
            }

            ImGui::SameLine();
        }

        ImGui::Text("= %02X", inData);        

        auto& speaker = m_controller.umpk().speaker();
        static int da = 0;
        ImGui::PlotLines(
            "Speaker",
            [](void* data, int pos){
                bool* d = (bool*)data;
                bool val = d[pos];

                printf("%lf\n", (float)val);

                return (float)val;
            },
            (void*)speaker.getBuffer(), 
            speaker.getSize(),
            0,
            0, 
            0.0f, 
            1.0f, 
            ImVec2(0, 80.0f)
        );
        da += 256;
    }

    void setOut(uint8_t value) { uint8ToVec(value, vecOut); }

private:
    Controller &m_controller;

    bool vecIn[8] = {0};
    bool vecOut[8] = {0};

    uint8_t vecToUint8(bool *vec) {
        uint8_t res = 0;

        for (int i = 0; i < 8; i++) {
            res = (res << 1) | (vec[i] & 1);
        }

        return res;
    }

    void uint8ToVec(uint8_t data, bool *outVec) {
        for (int i = 0; i < 8; i++) {
            outVec[7 - i] = (data >> i) & 0b1;
        }
    }
};

#endif // UI_IO_REGISTER_HPP
