#pragma once

#include <cinttypes>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Sfml8SegmentDigit {
    public:
        Sfml8SegmentDigit(sf::Vector2f pos) : _pos(pos) {
            const int segThick  = 4;
            const int segLength = 24;
            const int segSlant = 4;
            
            _parts[A].setPointCount(6);
            _parts[A].setPoint(0, sf::Vector2f(0,segThick/2));
            _parts[A].setPoint(1, sf::Vector2f(segThick/2,segThick));
            _parts[A].setPoint(2, sf::Vector2f(segLength-segThick/2,segThick));
            _parts[A].setPoint(3, sf::Vector2f(segSlant/2+segLength,segThick/2));
            _parts[A].setPoint(4, sf::Vector2f(segSlant/2+segLength-segThick/2,0));
            _parts[A].setPoint(5, sf::Vector2f(segSlant/2+segThick/2,0));

            _parts[G].setPointCount(6);
            _parts[G].setPoint(0, sf::Vector2f(0,segThick/2));
            _parts[G].setPoint(1, sf::Vector2f(segThick/2,segThick));
            _parts[G].setPoint(2, sf::Vector2f(segLength-segThick/2,segThick));
            _parts[G].setPoint(3, sf::Vector2f(segSlant/2+segLength,segThick/2));
            _parts[G].setPoint(4, sf::Vector2f(segSlant/2+segLength-segThick/2,0));
            _parts[G].setPoint(5, sf::Vector2f(segSlant/2+segThick/2,0));

            _parts[D].setPointCount(6);
            _parts[D].setPoint(0, sf::Vector2f(0,segThick/2));
            _parts[D].setPoint(1, sf::Vector2f(segThick/2,segThick));
            _parts[D].setPoint(2, sf::Vector2f(segLength-segThick/2,segThick));
            _parts[D].setPoint(3, sf::Vector2f(segSlant/2+segLength,segThick/2));
            _parts[D].setPoint(4, sf::Vector2f(segSlant/2+segLength-segThick/2,0));
            _parts[D].setPoint(5, sf::Vector2f(segSlant/2+segThick/2,0));

            _parts[F].setPointCount(6);
            _parts[F].setPoint(0, sf::Vector2f(segSlant+segThick/2,0));
            _parts[F].setPoint(1, sf::Vector2f(segSlant+segThick,segThick/2));
            _parts[F].setPoint(2, sf::Vector2f(segThick,segLength-segThick/2));
            _parts[F].setPoint(3, sf::Vector2f(segThick/2-segSlant/2,segLength));
            _parts[F].setPoint(4, sf::Vector2f(0,segLength-segThick/2));
            _parts[F].setPoint(5, sf::Vector2f(segSlant+0,segThick/2));

            _parts[E].setPointCount(6);
            _parts[E].setPoint(0, sf::Vector2f(segSlant+segThick/2,0));
            _parts[E].setPoint(1, sf::Vector2f(segSlant+segThick,segThick/2));
            _parts[E].setPoint(2, sf::Vector2f(segThick,segLength-segThick/2));
            _parts[E].setPoint(3, sf::Vector2f(segThick/2-segSlant/2,segLength));
            _parts[E].setPoint(4, sf::Vector2f(0,segLength-segThick/2));
            _parts[E].setPoint(5, sf::Vector2f(segSlant+0,segThick/2));
            
            _parts[B].setPointCount(6);
            _parts[B].setPoint(0, sf::Vector2f(segSlant+segThick/2+segSlant/2,0));
            _parts[B].setPoint(1, sf::Vector2f(segSlant+segThick,segThick/2));
            _parts[B].setPoint(2, sf::Vector2f(segThick,segLength-segThick/2));
            _parts[B].setPoint(3, sf::Vector2f(segThick/2,segLength));
            _parts[B].setPoint(4, sf::Vector2f(0,segLength-segThick/2));
            _parts[B].setPoint(5, sf::Vector2f(segSlant+0,segThick/2));

            _parts[C].setPointCount(6);
            _parts[C].setPoint(0, sf::Vector2f(segSlant+segThick/2+segSlant/2,0));
            _parts[C].setPoint(1, sf::Vector2f(segSlant+segThick,segThick/2));
            _parts[C].setPoint(2, sf::Vector2f(segThick,segLength-segThick/2));
            _parts[C].setPoint(3, sf::Vector2f(segThick/2,segLength));
            _parts[C].setPoint(4, sf::Vector2f(0,segLength-segThick/2));
            _parts[C].setPoint(5, sf::Vector2f(segSlant+0,segThick/2));

            _parts[H].setPointCount(5);
            _parts[H].setPoint(0, sf::Vector2f(segSlant+segThick/2,0));
            _parts[H].setPoint(1, sf::Vector2f(segThick+segSlant,0));
            _parts[H].setPoint(2, sf::Vector2f(segThick+segSlant,segThick));
            _parts[H].setPoint(3, sf::Vector2f(0,segThick*2));
            _parts[H].setPoint(4, sf::Vector2f(segSlant,segThick));

            int drawerX;
            int drawerY;

            drawerX = pos.x +segSlant/2 + segSlant*3/2;
            drawerY = pos.y + segThick*3/4;
            _parts[F].setPosition(drawerX, drawerY);

            drawerX += -segSlant*3/2;
            drawerY += segThick/2 + segLength;
            _parts[E].setPosition(drawerX, drawerY);
            
            drawerX += segLength + segSlant*3/2;
            drawerY = pos.y + segThick*3/4;
            _parts[B].setPosition(drawerX, drawerY);
            
            drawerX += -segSlant*3/2;
            drawerY += segThick/2 + segLength;
            _parts[C].setPosition(drawerX, drawerY);

            drawerX = pos.x +segSlant/2 + segThick/2 + segSlant*5/2;
            drawerY = pos.y;
            _parts[A].setPosition(drawerX, drawerY);
            
            drawerX += -segSlant*3/2;
            drawerY += segThick + segLength - segThick/2;
            _parts[G].setPosition(drawerX, drawerY);
            
            drawerX += -segSlant*3/2;
            drawerY += segThick/2 + segLength;
            _parts[D].setPosition(drawerX, drawerY);

            drawerX += segLength+segThick/4;
            drawerY -= 2 * segThick/4;
            _parts[H].setPosition(drawerX, drawerY);

            clear();
        }

        enum Segments { H, G, F, E, D, C, B, A };

        void lightUp(int segment) {
            _parts[segment].setFillColor(sf::Color(0x00FF7FFF));
        }

        void lightDown(int segment) {
            _parts[segment].setFillColor(sf::Color(0x0A5F384A));
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


        void draw(sf::RenderWindow& wnd) {
            for (int s = 0; s < 8; s++) {
                wnd.draw(_parts[s]);
            }
        }

    private:
        sf::Vector2f _pos;
        sf::ConvexShape _parts[8];
};

class Sfml8SegmentDisplay {

};