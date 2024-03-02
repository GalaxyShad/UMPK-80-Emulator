#ifndef UI_IO_REGISTER_HPP
#define UI_IO_REGISTER_HPP

#include "../irenderable.hpp"

#include <SFML/Graphics.hpp>
#include <functional>
#include <imgui.h>
#include <string>
#include <vector>

#include "../../controller.hpp"

struct UiIoRegisterEmits {
    std::function<void(uint8_t)> emitInRegisterChange;
};

class UiIoRegister : public IRenderable {
public:
    UiIoRegister(Controller &controller, UiIoRegisterEmits emits)
        : m_emits(emits), m_controller(controller) {}

    void render() override {
        for (uint8_t i = 0x80, c = 7; i != 0; i >>= 1, c--) {
            ImGui::RadioButton(("o" + std::to_string(c)).c_str(),
                               m_controller.port5Out() & i);
            ImGui::SameLine();
        }

        ImGui::NewLine();
        ImGui::Separator();

        for (int i = 0; i < 8; i++) {
            if (ImGui::Checkbox(("i" + std::to_string(7 - i)).c_str(),
                                &vecIn[i])) {
                m_emits.emitInRegisterChange(vecToUint8(vecIn));
            }

            ImGui::SameLine();
        }

        m_controller.port5In(vecToUint8(vecIn));
    }

    void setOut(uint8_t value) { uint8ToVec(value, vecOut); }

private:
    UiIoRegisterEmits m_emits;
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
