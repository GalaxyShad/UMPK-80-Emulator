#include "../../irenderable.hpp"
#include "ui-display-digit.hpp"
#include "../../../controller.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

#include <vector>

class UiDisplay : public IRenderable {
public: 
    UiDisplay(Controller& controller) : m_controller(controller) {
        rt.create(336, 64);
        
        for (int i = 0; i < 6; i++) {
            m_digitList.push_back(new UiDisplayDigit(
                UiDisplayDigitProps().setX(i * ((i > 3) ? 52 : 48))
            ));
        }
    }

    virtual ~UiDisplay() {
        for (auto& i : m_digitList) {
            delete i;
        }
    }

    void render() override {

        for (int d = 0; d < m_digitList.size(); d++) {
            m_digitList[d]->clear();
            m_digitList[d]->setValue(m_controller.getDisplayDigit(d));
            m_digitList[d]->draw(rt);
        }
        
        rt.display();
        
        ImGui::SetNextItemWidth(-100);
        ImGui::Image(rt); 
    }
private:
    std::vector<UiDisplayDigit*> m_digitList;
    Controller& m_controller;
    sf::RenderTexture rt;
};