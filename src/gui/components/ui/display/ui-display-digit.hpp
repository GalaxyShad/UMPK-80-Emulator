#include "../../irenderable.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

struct UiDisplayDigitProps {
    int thickness   = 4;
    int length      = 24;
    int slant       = 4;
    uint32_t lightupColor   = 0x00FF7FFF;
    uint32_t lightdownColor = 0x000000FF;

    float x = 0;
    float y = 0;

    UiDisplayDigitProps& setThickness(int value) { thickness = value; return *this; }
    UiDisplayDigitProps& setLength(int value) { length = value; return *this; }
    UiDisplayDigitProps& setSlant(int value) { slant = value; return *this; }
    UiDisplayDigitProps& setLightupColor(uint32_t value) { lightupColor = value; return *this; }
    UiDisplayDigitProps& setLightdownColor(uint32_t value) { lightdownColor = value; return *this; }
    UiDisplayDigitProps& setX(float value) { x = value; return *this; }
    UiDisplayDigitProps& setY(float value) { y = value; return *this; }
};

class UiDisplayDigit : public IRenderable {
public: 
    enum UiDisplayDigitSegments { H, G, F, E, D, C, B, A };

    UiDisplayDigit(UiDisplayDigitProps props): m_pos(sf::Vector2f(props.x, props.y)) {
        m_props = props;

        m_parts[A].setPointCount(6);
        m_parts[A].setPoint(0, sf::Vector2f(0,props.thickness/2));
        m_parts[A].setPoint(1, sf::Vector2f(props.thickness/2,props.thickness));
        m_parts[A].setPoint(2, sf::Vector2f(props.length-props.thickness/2,props.thickness));
        m_parts[A].setPoint(3, sf::Vector2f(props.slant/2+props.length,props.thickness/2));
        m_parts[A].setPoint(4, sf::Vector2f(props.slant/2+props.length-props.thickness/2,0));
        m_parts[A].setPoint(5, sf::Vector2f(props.slant/2+props.thickness/2,0));

        m_parts[G].setPointCount(6);
        m_parts[G].setPoint(0, sf::Vector2f(0,props.thickness/2));
        m_parts[G].setPoint(1, sf::Vector2f(props.thickness/2,props.thickness));
        m_parts[G].setPoint(2, sf::Vector2f(props.length-props.thickness/2,props.thickness));
        m_parts[G].setPoint(3, sf::Vector2f(props.slant/2+props.length,props.thickness/2));
        m_parts[G].setPoint(4, sf::Vector2f(props.slant/2+props.length-props.thickness/2,0));
        m_parts[G].setPoint(5, sf::Vector2f(props.slant/2+props.thickness/2,0));

        m_parts[D].setPointCount(6);
        m_parts[D].setPoint(0, sf::Vector2f(0,props.thickness/2));
        m_parts[D].setPoint(1, sf::Vector2f(props.thickness/2,props.thickness));
        m_parts[D].setPoint(2, sf::Vector2f(props.length-props.thickness/2,props.thickness));
        m_parts[D].setPoint(3, sf::Vector2f(props.slant/2+props.length,props.thickness/2));
        m_parts[D].setPoint(4, sf::Vector2f(props.slant/2+props.length-props.thickness/2,0));
        m_parts[D].setPoint(5, sf::Vector2f(props.slant/2+props.thickness/2,0));

        m_parts[F].setPointCount(6);
        m_parts[F].setPoint(0, sf::Vector2f(props.slant+props.thickness/2,0));
        m_parts[F].setPoint(1, sf::Vector2f(props.slant+props.thickness,props.thickness/2));
        m_parts[F].setPoint(2, sf::Vector2f(props.thickness,props.length-props.thickness/2));
        m_parts[F].setPoint(3, sf::Vector2f(props.thickness/2-props.slant/2,props.length));
        m_parts[F].setPoint(4, sf::Vector2f(0,props.length-props.thickness/2));
        m_parts[F].setPoint(5, sf::Vector2f(props.slant+0,props.thickness/2));

        m_parts[E].setPointCount(6);
        m_parts[E].setPoint(0, sf::Vector2f(props.slant+props.thickness/2,0));
        m_parts[E].setPoint(1, sf::Vector2f(props.slant+props.thickness,props.thickness/2));
        m_parts[E].setPoint(2, sf::Vector2f(props.thickness,props.length-props.thickness/2));
        m_parts[E].setPoint(3, sf::Vector2f(props.thickness/2-props.slant/2,props.length));
        m_parts[E].setPoint(4, sf::Vector2f(0,props.length-props.thickness/2));
        m_parts[E].setPoint(5, sf::Vector2f(props.slant+0,props.thickness/2));
        
        m_parts[B].setPointCount(6);
        m_parts[B].setPoint(0, sf::Vector2f(props.slant+props.thickness/2+props.slant/2,0));
        m_parts[B].setPoint(1, sf::Vector2f(props.slant+props.thickness,props.thickness/2));
        m_parts[B].setPoint(2, sf::Vector2f(props.thickness,props.length-props.thickness/2));
        m_parts[B].setPoint(3, sf::Vector2f(props.thickness/2,props.length));
        m_parts[B].setPoint(4, sf::Vector2f(0,props.length-props.thickness/2));
        m_parts[B].setPoint(5, sf::Vector2f(props.slant+0,props.thickness/2));

        m_parts[C].setPointCount(6);
        m_parts[C].setPoint(0, sf::Vector2f(props.slant+props.thickness/2+props.slant/2,0));
        m_parts[C].setPoint(1, sf::Vector2f(props.slant+props.thickness,props.thickness/2));
        m_parts[C].setPoint(2, sf::Vector2f(props.thickness,props.length-props.thickness/2));
        m_parts[C].setPoint(3, sf::Vector2f(props.thickness/2,props.length));
        m_parts[C].setPoint(4, sf::Vector2f(0,props.length-props.thickness/2));
        m_parts[C].setPoint(5, sf::Vector2f(props.slant+0,props.thickness/2));

        m_parts[H].setPointCount(5);
        m_parts[H].setPoint(0, sf::Vector2f(props.slant+props.thickness/2,0));
        m_parts[H].setPoint(1, sf::Vector2f(props.thickness+props.slant,0));
        m_parts[H].setPoint(2, sf::Vector2f(props.thickness+props.slant,props.thickness));
        m_parts[H].setPoint(3, sf::Vector2f(0,props.thickness*2));
        m_parts[H].setPoint(4, sf::Vector2f(props.slant,props.thickness));

        int drawerX;
        int drawerY;

        drawerX = m_pos.x + props.slant/2 + props.slant*3/2;
        drawerY = m_pos.y + props.thickness*3/4;
        m_parts[F].setPosition(drawerX, drawerY);

        drawerX += -props.slant*3/2;
        drawerY += props.thickness/2 + props.length;
        m_parts[E].setPosition(drawerX, drawerY);
        
        drawerX += props.length + props.slant*3/2;
        drawerY = m_pos.y + props.thickness*3/4;
        m_parts[B].setPosition(drawerX, drawerY);
        
        drawerX += -props.slant*3/2;
        drawerY += props.thickness/2 + props.length;
        m_parts[C].setPosition(drawerX, drawerY);

        drawerX = m_pos.x +props.slant/2 + props.thickness/2 + props.slant*5/2;
        drawerY = m_pos.y;
        m_parts[A].setPosition(drawerX, drawerY);
        
        drawerX += -props.slant*3/2;
        drawerY += props.thickness + props.length - props.thickness/2;
        m_parts[G].setPosition(drawerX, drawerY);
        
        drawerX += -props.slant*3/2;
        drawerY += props.thickness/2 + props.length;
        m_parts[D].setPosition(drawerX, drawerY);

        drawerX += props.length+props.thickness/4;
        drawerY -= 2 * props.thickness/4;
        m_parts[H].setPosition(drawerX, drawerY);

        clear();
    }

    void render() override {
        
    }

    void lightUp(int segment) {
        m_parts[segment].setFillColor(sf::Color(m_props.lightupColor));
    }

    void lightDown(int segment) {
        m_parts[segment].setFillColor(sf::Color(m_props.lightdownColor));
    }

    void clear() {
        for (int s = 0; s < 8; s++) lightDown(s);
    }

    void setValue(uint8_t newValue) {
        clear();

        if (newValue & 0b10000000) lightUp(0); 
        if (newValue & 0b01000000) lightUp(1); 
        if (newValue & 0b00100000) lightUp(2); 
        if (newValue & 0b00010000) lightUp(3); 

        if (newValue & 0b00001000) lightUp(4); 
        if (newValue & 0b00000100) lightUp(5); 
        if (newValue & 0b00000010) lightUp(6); 
        if (newValue & 0b00000001) lightUp(7); 
    }

    void draw(sf::RenderTarget& rt) {
        for (int s = 0; s < 8; s++) {
            rt.draw(m_parts[s]);
        }
    }
private:
    UiDisplayDigitProps m_props;
    sf::Vector2f m_pos;
    sf::ConvexShape m_parts[8];
};