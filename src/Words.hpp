#pragma once

#include "SFML/Graphics.hpp"
#include <string>

struct Word {
    sf::Text textLabel; //  holds SFML text (font, string, color, position, etc.)
    std::string textString;
    float fadeTimer;
    bool isFading;
    
    Word(const std::string& s, const sf::Font& f, float x, float size);
    void updateFontSize(float newSize);
};