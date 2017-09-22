#pragma once
#include<string>
#include<vector>
#include <Lengine/Camera.h>
class Player;
class Box;
class Light;

class LevelWriterNReader
{
public:
	static void saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
	static void saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights, const std::vector<unsigned char>& frameRawData, const int& width, const int& height);
	
	static void readAsText(const std::string& filePath,Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
	static void readAsBinary(const std::string& filePath, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights,  std::vector<unsigned char>& frameRawData, int& width, int& height);
private:
	static void saveTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights);
	static void saveAsBinaryV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights, const std::vector<unsigned char>& frameRawData,const int& width,const int& height);

	static void readTextV0(std::ifstream& fileIn, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights);
	static void readAsBinaryV0(std::ifstream& fin, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights, std::vector<unsigned char>& frameRawData, int& width, int& height);
};

