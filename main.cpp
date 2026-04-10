#include "src/Main.hpp"
#include "src/Config.hpp"
#include "src/Words.hpp"
#include "src/AlignCenter.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib> // std::srand()
#include <ctime> // std::time(nullptr)
#include <cctype>  // std::isprint(ch)
#include <cstdint>  // to fade words (animation)

using namespace std;

auto main() -> int {

    // Init
    srand(unsigned(time(nullptr)));

    if (!std::ifstream{configFolderPath + "config.txt"}) saveConfig();

    loadConfig("config.txt");

    auto wordList = loadWords("words.txt");
    if (wordList.empty()) {
        std::cerr << "Failed to load words from " << configFolderPath << " words.txt\n";
        return 0;
    }

    // Window
    sf::RenderWindow window{
            sf::VideoMode{sf::Vector2u{windowWidth, windowHeight}},
            "MonkeyTyper",
            sf::Style::Default,
            sf::State::Windowed,
            sf::ContextSettings{.antiAliasingLevel = 8}
    };
    window.setFramerateLimit(60); // FPS (frames per second)

    // Font
    sf::Font fontGame;
    if (!fontGame.openFromFile("../Assets/CalSans-Regular.ttf")) {
        return -1;
    }

    // Stop Game Text
    sf::Text stopGameText(fontGame, "Game Over!", 40);

    // Start Again Subtitle
    sf::Text restartGameText(fontGame, "Start again", 30);

    // Top Score Text
    std::string topScoreString = "Top Score: ";
    string scoreToComapare = loadHighScores("scores.txt");
    topScoreString += loadHighScores("scores.txt");
    auto topScore = sf::Text(fontGame, topScoreString, 24);
    topScore.setFillColor(sf::Color::Green);
    alignCenter(topScore, window);
    topScore.move({0.f, 220.f});

    // Start game menu
    sf::RectangleShape startGameMenu;
    startGameMenu.setSize(sf::Vector2f(window.getSize().x - 200, window.getSize().y - 200));
    startGameMenu.setFillColor(panelColor);
    startGameMenu.setOutlineThickness(2);
    startGameMenu.setOutlineColor({224, 164, 88});
    startGameMenu.setPosition({100, 100});

    // footer panel
    auto footer = sf::RectangleShape({float(windowWidth), float(panelHeight)});
    footer.setFillColor(panelColor);
    footer.setPosition({0.f, float(windowHeight - panelHeight)});

    // input words
    auto inputWords = sf::Text(fontGame, "", 32);
    inputWords.setFillColor(accentColor);
    inputWords.setPosition({windowWidth / 2.f, windowHeight - panelHeight / 2.f - 17.f});

    // Game INFO
    string currentInput;
    int currentScore = 0;
    int livesCount = initialLives;
    float spawnTimer = 0;
    vector<Word> fallingWords;
    vector<Word> fadingWords; // words to be removed
    bool gameOver = false;
    bool paused = false;
    bool skipText = true;
    sf::Clock gameClock;

    // Restart function
    auto restartGame = [&]() -> void {
        currentScore = 0;
        livesCount = initialLives;
        fallingWords.clear();
        fadingWords.clear();
        currentInput.clear();
        inputWords.setString("");
        spawnTimer = 0;
        gameOver = false;
        paused = false;
        skipText = true;
        scoreToComapare = loadHighScores("scores.txt");
        topScoreString = "Top Score: " + scoreToComapare;
        topScore.setString(topScoreString);
    };

    // Game loop
    while (window.isOpen()) {
        float deltaTime = gameClock.restart().asSeconds();

        // Events
        while (auto eventOpt = window.pollEvent()) {
            auto &currentEvent = *eventOpt;

            if (currentEvent.is<sf::Event::Closed>()) {
                window.close();
            } else if (currentEvent.is<sf::Event::KeyPressed>()) {
                auto keyCode = currentEvent.getIf<sf::Event::KeyPressed>()->code;
                if (keyCode == sf::Keyboard::Key::Tab && !gameOver)
                    paused = !paused;
                else if (keyCode == sf::Keyboard::Key::Escape)
                    window.close();
                else if ((gameOver || paused) && keyCode == sf::Keyboard::Key::R)
                    restartGame();
                else if (keyCode == sf::Keyboard::Key::Enter) {
                    currentInput.clear();
                    inputWords.setString("");
                    auto lb = inputWords.getLocalBounds();
                    inputWords.setOrigin({lb.size.x / 2.f, 0.f}); // center
                }

                if (keyCode == sf::Keyboard::Key::F1) startSpeed = max(40, startSpeed - 10);
                if (keyCode == sf::Keyboard::Key::F2) startSpeed += 10;
                if (keyCode == sf::Keyboard::Key::F3) spawnInterval = max(1, spawnInterval - 1);
                if (keyCode == sf::Keyboard::Key::F4) spawnInterval += 1;

                int oldFallingWordsSize = fallingWordsSize;
                if (keyCode == sf::Keyboard::Key::F5) fallingWordsSize = max(8, fallingWordsSize - 2);
                if (keyCode == sf::Keyboard::Key::F6) fallingWordsSize += 2;

                // Update font size
                if (oldFallingWordsSize != fallingWordsSize &&
                    (keyCode == sf::Keyboard::Key::F5 || keyCode == sf::Keyboard::Key::F6)) {
                    float newSize = static_cast<float>(fallingWordsSize);
                    for (auto &word: fallingWords) {
                        word.updateFontSize(newSize);
                    }
                }

                if (keyCode >= sf::Keyboard::Key::F1 && keyCode <= sf::Keyboard::Key::F6) {
                    saveConfig();
                }
            } else if (currentEvent.is<sf::Event::TextEntered>()) {
                if (skipText) skipText = false;
                else if (!gameOver && !paused) {
                    char inputChar = char(currentEvent.getIf<sf::Event::TextEntered>()->unicode);
                    // removing with backspace
                    if (inputChar == '\b' && !currentInput.empty()) {
                        currentInput.pop_back();
                    }
                        // ignores control keys
                        else if (isprint((char) inputChar)) {
                        currentInput += inputChar;
                    }

                    inputWords.setString(currentInput);
                    auto localBounds = inputWords.getLocalBounds();
                    inputWords.setOrigin({localBounds.size.x / 2.f, 0.f});

                    auto matchedWordIter = find_if(fallingWords.begin(), fallingWords.end(),
                                                   [&](const auto &currentWord) {
                                                       return currentWord.textString == currentInput;
                                                   });

                    if (matchedWordIter != fallingWords.end()) {
                        matchedWordIter->isFading = true;
                        matchedWordIter->textLabel.setFillColor(inverseAccentColor);
                        fadingWords.push_back(*matchedWordIter);
                        fallingWords.erase(matchedWordIter);
                        currentInput.clear();
                        inputWords.setString("");
                        ++currentScore;
                    }
                }
            } else if (currentEvent.is<sf::Event::MouseMoved>()) {
                // inside your MouseMoved branch:
                sf::Vector2i mousePixel = sf::Mouse::getPosition(
                        window); // By default, mapPixelToCoords uses the current view
                sf::Vector2f mouseF = window.mapPixelToCoords(mousePixel);

                // reset both to their defaults first
                stopGameText.setFillColor(sf::Color::Red);
                restartGameText.setFillColor(sf::Color::White);

                if (paused) {
                    // highlight the “Resume” text
                    if (stopGameText.getGlobalBounds().contains(mouseF)) {
                        stopGameText.setFillColor(sf::Color::Green);
                    } else if (restartGameText.getGlobalBounds().contains(mouseF)) {
                        restartGameText.setFillColor((sf::Color::Green));
                    }
                } else if (gameOver) {
                    // highlight the “Start again” text
                    if (restartGameText.getGlobalBounds().contains(mouseF)) {
                        restartGameText.setFillColor(sf::Color::Green);
                    }
                }
            } else if (currentEvent.is<sf::Event::MouseButtonPressed>()) {
                auto mb = currentEvent.getIf<sf::Event::MouseButtonPressed>();
                if (mb->button == sf::Mouse::Button::Left) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(mb->position);
                    if (restartGameText.getGlobalBounds().contains(worldPos)) {
                        restartGame();
                    }
                    // 1 if we’re paused and clicked “Resume”, un-pause
                    if (paused && stopGameText.getGlobalBounds().contains(worldPos)) {
                        paused = false;
                    }
                        // 2 if we’re game-over and clicked “Start again”, restart
                    else if (gameOver && restartGameText.getGlobalBounds().contains(worldPos)) {
                        restartGame();
                    }
                }
            }
        }

        // Update
        if (!gameOver && !paused) {
            // Spawn words
            spawnTimer += deltaTime;
            if (spawnTimer >= spawnInterval) {
                if (!wordList.empty()) {
                    string randomWord = wordList[rand() % wordList.size()];

                    sf::Text tempText(fontGame, randomWord, static_cast<unsigned>(fallingWordsSize));
                    float wordWidth = tempText.getLocalBounds().size.x;

                    float maxX = windowWidth - wordWidth - 5.0f;
                    float minX = 5.0f;
                    float xPosition = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);

                    fallingWords.emplace_back(
                            randomWord,
                            fontGame,
                            xPosition,
                            float(fallingWordsSize)
                    );
                }
                spawnTimer = 0;
            }

            // Move words
            for (auto &currentWord: fallingWords) currentWord.textLabel.move({0, startSpeed * deltaTime});

            // Check bottom hits
            auto wordIterator = fallingWords.begin();
            while (wordIterator != fallingWords.end()) {
                float wordY = wordIterator->textLabel.getPosition().y + wordIterator->textLabel.getCharacterSize();
                if (wordY >= windowHeight - panelHeight) {
                    --livesCount;
                    // remove it from the listy
                    wordIterator = fallingWords.erase(wordIterator);
                } else {
                    ++wordIterator;
                }
            }

            // Check game over
            if (livesCount <= 0) {
                gameOver = true;
                if (currentScore > std::stoi(scoreToComapare)) {
                    saveHighScores("scores.txt", currentScore);
                    scoreToComapare = std::to_string(currentScore);
                    std::string newTopScore = "Top Score: " + scoreToComapare;
                    topScore.setString(newTopScore);
                }
            }

            // Fading effect
            for (auto &fadingWord: fadingWords) {
                fadingWord.fadeTimer -= deltaTime;
                auto currentColor = fadingWord.textLabel.getFillColor();
                currentColor.a = uint8_t(255 * fadingWord.fadeTimer /
                                         0.3f); // unsigned integer type with width of exactly 8, 16, 32 and 64 bits respectively
                fadingWord.textLabel.setFillColor(currentColor);
            }

            // Remove faded words
            erase_if(fadingWords, [](const auto &fadingWord) { return fadingWord.fadeTimer <= 0; });
        }

        // Render
        window.clear(backgroundColor);

        // Draw words
        for (auto &currentWord: fallingWords) {
            if (!currentInput.empty() && currentWord.textString.rfind(currentInput, 0) == 0) { // rfind() reverse find
                sf::Text prefixText{fontGame, currentInput, currentWord.textLabel.getCharacterSize()};
                prefixText.setFillColor(highlightColor);
                prefixText.setPosition(currentWord.textLabel.getPosition());
                window.draw(prefixText);
                auto suffixPosition = currentWord.textLabel.findCharacterPos(currentInput.size());
                sf::Text suffixText{fontGame, currentWord.textString.substr(currentInput.size()),
                                    currentWord.textLabel.getCharacterSize()};
                suffixText.setFillColor(textColor);
                suffixText.setPosition(suffixPosition);
                window.draw(suffixText);
            } else {
                window.draw(currentWord.textLabel);
            }
        }

        // Fading words
        for (auto &fadingWord: fadingWords) window.draw(fadingWord.textLabel);

        // UI
        window.draw(footer);
        window.draw(inputWords);

        // draw score
        {
            sf::Text scoreDisplay{fontGame, "score: " + to_string(currentScore), 24u};
            scoreDisplay.setFillColor(accentColor);
            scoreDisplay.setPosition({10.f, 560.f});
            window.draw(scoreDisplay);
        }
        // draw lives
        for (int i = 0; i < livesCount; ++i) {
            sf::CircleShape heartShape{10.f};
            heartShape.setFillColor(accentColor);
            heartShape.setPosition({float(windowWidth - (i + 1) * 25 - 10), 570.f});
            window.draw(heartShape);
        }

        // Pause/game over overlay
        if (gameOver || paused) {
            window.draw(startGameMenu);
            // speed
            std::ostringstream speedS;
            speedS << "Speed: (" << startSpeed << ") [F1/F2]";
            // interval
            std::ostringstream intervalS;
            intervalS << "Spawn interval: (" << spawnInterval << ") [F3/F4]";
            // font
            std::ostringstream fontS;
            fontS << "Font Size: (" << fallingWordsSize << ") [F5/F6]";
            if (gameOver) {
                // Game-over menu

                stopGameText.setString("Game Over!");
                alignCenter(stopGameText, window);
                stopGameText.move({0.f, -30.f});
                window.draw(stopGameText);

                restartGameText.setString("Start Again");
                alignCenter(restartGameText, window);
                restartGameText.move({0.f, 35.f});
                window.draw(restartGameText);

                sf::Text speedSetting(fontGame, speedS.str(), 20);
                alignCenter(speedSetting, window);
                speedSetting.move({0.f, 95.f});
                window.draw(speedSetting);

                sf::Text intervalSetting(fontGame, intervalS.str(), 20);
                alignCenter(intervalSetting, window);
                intervalSetting.move({0.f, 125.f});
                window.draw(intervalSetting);

                sf::Text fontSizeSetting(fontGame, fontS.str(), 20);
                alignCenter(fontSizeSetting, window);
                fontSizeSetting.move({0.f, 155.f});
                window.draw(fontSizeSetting);

                window.draw(topScore);
            } else {
                // Pause menu
                stopGameText.setString("Resume");
                alignCenter(stopGameText, window);
                stopGameText.move({0.f, -15.f});
                window.draw(stopGameText);

                restartGameText.setString("Start Again");
                alignCenter(restartGameText, window);
                restartGameText.move({0.f, 35.f});
                window.draw(restartGameText);

                sf::Text speedSetting(fontGame, speedS.str(), 20);
                alignCenter(speedSetting, window);
                speedSetting.move({0.f, 95.f});
                window.draw(speedSetting);

                sf::Text intervalSetting(fontGame, intervalS.str(), 20);
                alignCenter(intervalSetting, window);
                intervalSetting.move({0.f, 125.f});
                window.draw(intervalSetting);

                sf::Text fontSizeSetting(fontGame, fontS.str(), 20);
                alignCenter(fontSizeSetting, window);
                fontSizeSetting.move({0.f, 155.f});
                window.draw(fontSizeSetting);

                window.draw(topScore);
            }

            // shortcuts instructions draw
            sf::Text instructionsText{fontGame, "press [ R ] to restart    press [ Esc ] to exit", 24u};
            instructionsText.setFillColor(textColor);
            {
                auto instrBounds = instructionsText.getLocalBounds();
                instructionsText.setOrigin({instrBounds.size.x / 2.f, 0.f});
                instructionsText.setPosition({windowWidth / 2.f, windowHeight / 2.f + 220.f});
            }
            window.draw(instructionsText);
        }
        window.display();
    }
    return 0;
}