#ifndef UI_KEYBOARD_HPP
#define UI_KEYBOARD_HPP

#include <functional>
#include <imgui.h>
#include <utility>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"
#include "../../controller.hpp"
#include "SFML/Window/Keyboard.hpp"

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

    const std::map<sf::Keyboard::Key, KeyboardKey> m_realKeysMapping{
        {sf::Keyboard::Num0, KeyboardKey::_0},
        {sf::Keyboard::Num1, KeyboardKey::_1},
        {sf::Keyboard::Num2, KeyboardKey::_2},
        {sf::Keyboard::Num3, KeyboardKey::_3},
        {sf::Keyboard::Num4, KeyboardKey::_4},
        {sf::Keyboard::Num5, KeyboardKey::_5},
        {sf::Keyboard::Num6, KeyboardKey::_6},
        {sf::Keyboard::Num7, KeyboardKey::_7},
        {sf::Keyboard::Num8, KeyboardKey::_8},
        {sf::Keyboard::Num9, KeyboardKey::_9},
        {sf::Keyboard::A, KeyboardKey::_A},
        {sf::Keyboard::B, KeyboardKey::_B},
        {sf::Keyboard::C, KeyboardKey::_C},
        {sf::Keyboard::D, KeyboardKey::_D},
        {sf::Keyboard::E, KeyboardKey::_E},
        {sf::Keyboard::F, KeyboardKey::_F},

        {sf::Keyboard::Enter, KeyboardKey::P},
        {sf::Keyboard::Space, KeyboardKey::OT_A},
        {sf::Keyboard::Backspace, KeyboardKey::UM},

        {sf::Keyboard::Right, KeyboardKey::ZP_UV},
        {sf::Keyboard::Left, KeyboardKey::UM},
        {sf::Keyboard::Up, KeyboardKey::P},
        {sf::Keyboard::Down, KeyboardKey::ST},

        {sf::Keyboard::F5, KeyboardKey::P},
        {sf::Keyboard::F2, KeyboardKey::R},
    };

    void uiHandler() {
        for (int i = 0; i < M_KEYMAP_H; i++) {
            for (int j = 0; j < M_KEYMAP_W; j++) {
                const char* keyLabel = m_keyNamesMap[i][j].c_str();
                
                bool isEmpty = (i < 2 && j == 0);

                if (!isEmpty) {
                    ImGui::Button(keyLabel, ImVec2(32.0f, 32.0f));

                    m_uiKeysState[(int)m_keyMap[i][j]] = ImGui::IsItemActive();

                    ImGui::SameLine();
                } else {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 40);
                }
            }
            ImGui::NewLine();
        }
    }

    void realKeyboardHandler() {
        for (const auto& kv : m_realKeysMapping) {
            m_extKeysState[(int)kv.second] = sf::Keyboard::isKeyPressed(kv.first);
        }
    }
};

#endif // UI_KEYBOARD_HPP
