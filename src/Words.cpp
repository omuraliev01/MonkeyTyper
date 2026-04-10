#include "Words.hpp"
#include "Main.hpp"
#include <algorithm>

Word::Word(const std::string &s, const sf::Font &f, float x, float size)
        : textLabel(f, s, static_cast<unsigned>(size)), textString(s), fadeTimer(0.3f), isFading(false) {
    textLabel.setFillColor(textColor);

    // Measure the string so we can clamp its X position
    sf::Text tempText(f, s, static_cast<unsigned>(size));
    auto bounds = tempText.getLocalBounds();
    float wordWidth = bounds.size.x;

    // Clamp X so it never starts off-screen
    x = std::max(5.0f, std::min(x, windowWidth - wordWidth - 5.0f));
    textLabel.setPosition({x, -size});
}


void Word::updateFontSize(float newSize) {
    // current position
    sf::Vector2f currentPosition = textLabel.getPosition();
    // changing size
    unsigned int newSizeU = static_cast<unsigned>(newSize);
    textLabel.setCharacterSize(newSizeU);
    // reset origin position
    textLabel.setPosition(currentPosition);
}