#ifndef UI_KEYBOARD_HPP
#define UI_KEYBOARD_HPP

#include <imgui.h>
#include <utility>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"
#include "../../controller.hpp"

class UiKeyboard : public IRenderable {
public:
    UiKeyboard(Controller& controller) 
        : m_controller(controller) {}

    void render() override {
        uiHandler();
        realKeyboardHandler();

        for (int i = 0; i < 26; i++)
            m_controller.setUmpkKey((KeyboardKey)i, m_extKeysState[i] || m_uiKeysState[i]);
    }

private:
    Controller& m_controller;

    static const int M_KEYMAP_W = 7; 
    static const int M_KEYMAP_H = 4;

    struct KeyButton {
        std::string name;
        KeyboardKey key;
    };

    const std::string m_keyNamesMap[M_KEYMAP_H][M_KEYMAP_W] = {
        { " ",  "R",    "SHC",   "C", "D", "E", "F" },
        { " ",  "SHK",  "PrSch", "8", "9", "A", "B" },
        { "St", "OtRg", "OtA",   "4", "5", "6", "7" },
        { "P",  "Um",   "ZpUv",  "0", "1", "2", "3" }
    };

    bool m_uiKeysState[26] = { false };
    bool m_extKeysState[26] = { false };

    const KeyboardKey m_keyMap[M_KEYMAP_H][M_KEYMAP_W] = {
        { KeyboardKey::_0, KeyboardKey::R,     KeyboardKey::SHC, KeyboardKey::_C, KeyboardKey::_D, KeyboardKey::_E, KeyboardKey::_F  },
        { KeyboardKey::_0, KeyboardKey::SHK,   KeyboardKey::PR_SCH, KeyboardKey::_8, KeyboardKey::_9, KeyboardKey::_A, KeyboardKey::_B  },
        { KeyboardKey::ST, KeyboardKey::OT_RG, KeyboardKey::OT_A, KeyboardKey::_4, KeyboardKey::_5, KeyboardKey::_6, KeyboardKey::_7  },
        { KeyboardKey::P,  KeyboardKey::UM,    KeyboardKey::ZP_UV, KeyboardKey::_0, KeyboardKey::_1, KeyboardKey::_2, KeyboardKey::_3  }
    };

    const std::map<ImGuiKey, KeyboardKey> m_realKeysMapping{
        {ImGuiKey_0, KeyboardKey::_0},
        {ImGuiKey_1, KeyboardKey::_1},
        {ImGuiKey_2, KeyboardKey::_2},
        {ImGuiKey_3, KeyboardKey::_3},
        {ImGuiKey_4, KeyboardKey::_4},
        {ImGuiKey_5, KeyboardKey::_5},
        {ImGuiKey_6, KeyboardKey::_6},
        {ImGuiKey_7, KeyboardKey::_7},
        {ImGuiKey_8, KeyboardKey::_8},
        {ImGuiKey_9, KeyboardKey::_9},
        {ImGuiKey_A, KeyboardKey::_A},
        {ImGuiKey_B, KeyboardKey::_B},
        {ImGuiKey_C, KeyboardKey::_C},
        {ImGuiKey_D, KeyboardKey::_D},
        {ImGuiKey_E, KeyboardKey::_E},
        {ImGuiKey_F, KeyboardKey::_F},

        {ImGuiKey_Space, KeyboardKey::OT_A},

        {ImGuiKey_RightArrow, KeyboardKey::ZP_UV},
        {ImGuiKey_LeftArrow, KeyboardKey::UM},
        {ImGuiKey_UpArrow, KeyboardKey::P},
        {ImGuiKey_DownArrow, KeyboardKey::ST},
    };

    void uiHandler() {
        for (int i = 0; i < M_KEYMAP_H; i++) {
            for (int j = 0; j < M_KEYMAP_W; j++) {
                const char* keyLabel = m_keyNamesMap[i][j].c_str();
                
                bool isEmpty = (i < 2 && j == 0);

                if (!isEmpty) {
                    if (m_extKeysState[(int)m_keyMap[i][j]]) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
                    }

                    ImGui::Button(keyLabel, ImVec2(32.0f, 32.0f));

                    if (m_extKeysState[(int)m_keyMap[i][j]]) {
                        ImGui::PopStyleColor();
                    }

                    m_uiKeysState[(int)m_keyMap[i][j]] = ImGui::IsItemActive();

                    ImGui::SameLine();
                } else {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 40);
                }

                if (j == 2)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
            }
            ImGui::NewLine();
        }
    }

    void realKeyboardHandler() {
        if (ImGui::IsWindowFocused() == false)
            return;

        for (const auto& kv : m_realKeysMapping) {
            m_extKeysState[(int)kv.second] = ImGui::IsKeyDown(kv.first);
        }
    }
};

#endif // UI_KEYBOARD_HPP
