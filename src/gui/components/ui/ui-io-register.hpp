#include "../irenderable.hpp"

#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct UiIoRegisterEmits {
    std::function<void(uint8_t)> emitInRegisterChange;
};

class UiIoRegister : public IRenderable {
public:
    UiIoRegister(UiIoRegisterEmits emits) : m_emits(emits) {}

    void render() override {
        for (int i = 0; i < 8; i++) {
            ImGui::RadioButton(("o" + std::to_string(7-i)).c_str(), vecOut[i]);
            ImGui::SameLine();
        }

        ImGui::NewLine();
        ImGui::Separator();

        for (int i = 0; i < 8; i++) {
            if (ImGui::Checkbox(("i" + std::to_string(7-i)).c_str(), &vecIn[i])) {
                m_emits.emitInRegisterChange(vecToUint8(vecIn));
            }

            ImGui::SameLine();
        }
    }

    void setOut(uint8_t value) { uint8ToVec(value, vecOut); }
private:
    UiIoRegisterEmits m_emits;
    bool vecIn[8] = { 0 };
    bool vecOut[8] = { 0 };

    uint8_t vecToUint8(bool* vec) {
        uint8_t res = 0;
        
        for (int i = 0; i < 8; i++) {
            res = (res << 1) | (vec[i] & 1);
        }

        return res;
    }

    void uint8ToVec(uint8_t data, bool* outVec) {
        for (int i = 0; i < 8; i++) {
            outVec[7-i] = (data >> i) & 0b1;
        }
    }
};