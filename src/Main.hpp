#pragma once

#include "SFML/Graphics.hpp"
#include <vector>
#include <string>
#include "Config.hpp"
#include "Words.hpp"

// Constants
const auto windowWidth = 800;
const auto windowHeight = 600;
const auto panelHeight = 40;
const auto initialLives = 3;

const std::string configFolderPath = "../Assets/";

// Colors
const auto backgroundColor = sf::Color{30,  60,  45 };
const auto panelColor = sf::Color{27,  38,  59};
const auto textColor = sf::Color{240, 244, 239};
const auto accentColor = sf::Color{224, 164,  88};
const auto highlightColor = sf::Color{230,  57,  70};
const auto inverseAccentColor = sf::Color{75, 190, 235};