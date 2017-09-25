#pragma once
#include<string>
#include<vector>
#include <Lengine/Camera.h>
class Player;
class Boxes;
class Light;
class Lights;
class Tiles;
class LevelWriterNReader
{
public:
	static void saveAsText(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles, const Lights& lights);
	static void readAsText(const std::string& filePath, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights);
	
	static void saveAsBinary(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles, const Lights& lights);
	static void readAsBinary(const std::string& filePath, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights);

private:
	static void saveTextV0(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles,const Lights& lights);
	static void readTextV0(std::ifstream& fileIn, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights);

	static void saveAsBinaryV0(const std::string& filePath, const Player& player,const Boxes& boxes, const Tiles& tiles, const Lights& lights);
	static void readAsBinaryV0(std::ifstream& fin, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights);
};

