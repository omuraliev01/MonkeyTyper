#include "Config.hpp"
#include "Main.hpp"
#include "fmt/format.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

// Config settings
int startSpeed = 70;
int spawnInterval = 2;
int fallingWordsSize = 24;


vector<string> loadWords(const string &fileName) {
    string filePath = configFolderPath + fileName;
    ifstream in{filePath};
    vector<string> wordsList;
    for (string word; in >> word;)
        if (word.size() <= 7) wordsList.push_back(word);
    return wordsList;
}

void saveConfig() {
    string filePath = configFolderPath + "config.txt";
    auto out = ofstream{filePath};

    out << fmt::format(
            "startSpeed={}\n"
            "spawnInterval={}\n"
            "fallingWordsSize={}\n",
            startSpeed,
            spawnInterval,
            fallingWordsSize
    );
}

void loadConfig(const std::string &fileName) {
    string filePath = configFolderPath + fileName;
    ifstream in{filePath};
    if (!in) return;
    string line;
    while (getline(in, line)) {
        if (line.empty() || line.front() == '#') continue;
        auto pos = line.find('=');
        if (pos == string::npos) continue;
        auto key = line.substr(0, pos);
        auto val = line.substr(pos + 1);

        try {
            int v = stoi(val);
            if (key == "startSpeed") startSpeed = std::max(40, v);
            else if (key == "spawnInterval") spawnInterval = std::max(1, v);
            else if (key == "fallingWordsSize") fallingWordsSize = std::max(8, v);
        } catch (const std::exception &) {
        }

    }
}

string loadHighScores(const string &fileName) {
    string filePath = configFolderPath + fileName;
    ifstream in{filePath};
    string highScore = "";
    for (int score; in >> score;)
        highScore = to_string(score);
    return highScore;

}


void saveHighScores(const string &fileName, const int newScore) {
    string filePath = configFolderPath + fileName;
    std::ofstream out{filePath};
    out << to_string(newScore) << "\n";
}
