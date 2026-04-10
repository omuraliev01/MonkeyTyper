#include "AlignCenter.hpp"
#include <cmath>
#include "SFML/Graphics.hpp"

// Centering the text, according the window
void alignCenter(sf::Text &text, const sf::RenderWindow &window) {
    auto center = text.getGlobalBounds().size / 1.65f ;
    auto localBounds = center + text.getLocalBounds().position;
    auto rounded = sf::Vector2f{std::round(localBounds.x), std::round(localBounds.y + 40)};
    text.setOrigin(rounded);
    text.setPosition(sf::Vector2f(window.getSize() / 2u));
}