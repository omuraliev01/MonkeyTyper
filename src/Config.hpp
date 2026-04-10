#pragma once

#include <vector>
#include <string>

using namespace std;
// Settings
extern int startSpeed; // extern any source file that includes this will be able to use global int "stackoverflow"
extern int spawnInterval;
extern int fallingWordsSize;

// File operations
vector<std::string> loadWords(const std::string& fileName);
void saveConfig();
void loadConfig(const string& fileName);
string loadHighScores(const string& fileName);
void saveHighScores(const string& fileName, int newScore);