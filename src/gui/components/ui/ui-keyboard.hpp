#include <functional>
#include <imgui.h>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../irenderable.hpp"

struct UiKeyboardProps {
    float size = 32.f;
};

struct UiKeyboardEmits {
    std::function<void(const char* keyName)> emitKeyPress;
};

class UiKeyboard : public IRenderable {
public:
    UiKeyboard(UiKeyboardProps props, UiKeyboardEmits emits) 
        : m_props(props), m_emits(emits) {}

    void render() override {
        realKeyboardHandler();

        for (int i = 0; i < M_KEYMAP_H; i++) {
            for (int j = 0; j < M_KEYMAP_W; j++) {
                const char* keyLabel = m_keyMap[i][j].c_str();
                
                if (key(keyLabel))
                    m_emits.emitKeyPress(keyLabel);

                ImGui::SameLine();
            }
            ImGui::NewLine();
        }
    }

private:
    UiKeyboardProps m_props;
    UiKeyboardEmits m_emits;

    static const int M_KEYMAP_W = 7; 
    static const int M_KEYMAP_H = 4;

    const std::string m_keyMap[M_KEYMAP_H][M_KEYMAP_W] = {
        { " ",  "R",    "SHC",   "C", "D", "E", "F" },
        { " ",  "SHK",  "PrSch", "8", "9", "A", "B" },
        { "St", "OtRg", "OtA",   "4", "5", "6", "7" },
        { "P",  "Um",   "ZpUv",  "0", "1", "2", "3" }
    };

    const std::map<sf::Keyboard::Key, std::string> m_realKeysMapping{
        { sf::Keyboard::Num0,   "0"    },
        { sf::Keyboard::Num1,   "1"    },
        { sf::Keyboard::Num2,   "2"    },
        { sf::Keyboard::Num3,   "3"    },
        { sf::Keyboard::Num4,   "4"    },
        { sf::Keyboard::Num5,   "5"    },
        { sf::Keyboard::Num6,   "6"    },
        { sf::Keyboard::Num7,   "7"    },
        { sf::Keyboard::Num8,   "8"    },
        { sf::Keyboard::Num9,   "9"    },
        { sf::Keyboard::A,      "A"    },
        { sf::Keyboard::B,      "B"    },
        { sf::Keyboard::C,      "C"    },
        { sf::Keyboard::D,      "D"    },
        { sf::Keyboard::E,      "E"    },
        { sf::Keyboard::F,      "F"    },

        { sf::Keyboard::Enter,  "ZpUv" },
        { sf::Keyboard::Space,  "OtA"  },

        { sf::Keyboard::Right,  "ZpUv" },
        { sf::Keyboard::Left,   "Um"   },
        { sf::Keyboard::Up,     "P"    },
        { sf::Keyboard::Down,   "St"   },

        { sf::Keyboard::F5,     "P"    },
    };

    bool key(const char* label) {
        return ImGui::Button(label, ImVec2(m_props.size, m_props.size));
    }

    void realKeyboardHandler() {
        for (const auto& kv : m_realKeysMapping) {
            if (sf::Keyboard::isKeyPressed(kv.first))
                m_emits.emitKeyPress(kv.second.c_str());
        }
    }
};
