#include "../../irenderable.hpp"
#include "ui-display-digit.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

class UiDisplay : public IRenderable {
public: 
    UiDisplay(int digitCount) {
        rt.create(336, 64);
        
        for (int i = 0; i < digitCount; i++) {
            m_digitList.push_back(new UiDisplayDigit(
                UiDisplayDigitProps().setX(i * ((i > 3) ? 52 : 48))
            ));
        }
    }

    void setDigit(int digit, uint8_t value) {
        m_digitList[digit]->setValue(value);
    }

    void render() override {
        

        for (int d = 0; d < m_digitList.size(); d++) 
            m_digitList[d]->draw(rt);
        rt.display();
        
        ImGui::SetNextItemWidth(-100);
        ImGui::Image(rt); 
    }
private:
    std::vector<UiDisplayDigit*> m_digitList;
    sf::RenderTexture rt;
};