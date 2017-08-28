#pragma once
#include<string>
#include<vector>
class Player;
class Box;
class Light;
class LevelWriterNReader
{
public:
	static void saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
	static void saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
	static void readAsText(const std::string& filePath,Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
	static void readAsBinary(const std::string& filePath, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
private:
	static void saveTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
	static void readTextV0(std::ifstream& fileIn, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
};

