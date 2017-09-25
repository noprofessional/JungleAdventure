#include "LevelWriterNReader.h"

#include<fstream>
#include<Lengine/FatalError.h>
#include<Lengine/TextureCache.h>

#include"Player.h"
#include"Box.h"
#include"Tile.h"
#include"Light.h"
#include <Lengine/SpriteBatch.h>

// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;
const unsigned int BIN_VERSION_0 = 100;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;
const unsigned int BIN_VERSION = BIN_VERSION_0;

void LevelWriterNReader::saveAsText(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles,const Lights& lights) {
	saveTextV0(filePath, player, boxes, tiles, lights);
}
void LevelWriterNReader::readAsText(const std::string& filePath, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights) {
	std::ifstream fileIn(filePath);
	if (fileIn.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("File " + filePath + " can not be open!");
	}
	unsigned int version;
	fileIn >> version;
	switch (version) {
	case TEXT_VERSION_0:
		readTextV0(fileIn, player, boxes, tiles, lights);
		break;
	//	case otherVersion:
	//		readTextV...;
	//		break;
	default:
		Lengine::fatalError("Unknow version in leverl file!\nFile may have been corrupted...");
	}
}

void LevelWriterNReader::saveAsBinary(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles, const Lights& lights) {
	saveAsBinaryV0(filePath, player, boxes, tiles,lights);
}
void LevelWriterNReader::readAsBinary(const std::string& filePath, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights) {
	
	std::ifstream fileIn(filePath,std::ios_base::binary);
	if (fileIn.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("File " + filePath + " can not be open!");
	}

	unsigned int version;
	fileIn.read((char*)&version, sizeof(unsigned int));

	switch (version) {
	case BIN_VERSION_0:
		readAsBinaryV0(fileIn, player, boxes, tiles, lights);
		break;
//	case otherVersion:
//		readTextV...;
//		break;
	default:
		Lengine::fatalError("Unknow version in leverl file!\nFile may have been corrupted...");
	}
}

void LevelWriterNReader::saveTextV0(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles, const Lights& lights) {
	// Open file and Clear it
	std::ofstream fileOut(filePath,std::ios::trunc);
	if (fileOut.fail()) {
		//error check
		perror(filePath.c_str());
		Lengine::fatalError("file " + filePath + " open failed!\n");
	}

	//version
	fileOut << TEXT_VERSION<<'\n';

	//player
	player.writeAsText(fileOut);

	//boxes
	boxes.writeAsText(fileOut);

	//tiles
	tiles.writeAsText(fileOut);

	//lights
	lights.writeAsText(fileOut);

}
void LevelWriterNReader::readTextV0(std::ifstream& fileIn, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights) {
	//player load
	player.readFromText(fileIn);

	//boxes load
	boxes.readFromText(fileIn);

	//tiles load
	tiles.readFromText(fileIn);

	//lights load
	lights.readFromText(fileIn);

	fileIn.close();
}

void LevelWriterNReader::saveAsBinaryV0(const std::string& filePath, const Player& player, const Boxes& boxes, const Tiles& tiles, const Lights& lights) {
	
	std::ofstream fout(filePath.c_str(), std::ios_base::binary);
	if (fout.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("file " + filePath + " open failed!\n");
	}

	fout.write((char*)&BIN_VERSION, sizeof(unsigned int));

	player.writeAsBinary(fout);

	boxes.writeAsBinary(fout);

	tiles.writeAsBinary(fout);

	lights.writeAsBinary(fout);

	fout.close();
}
void LevelWriterNReader::readAsBinaryV0(std::ifstream& fin, Player& player, Boxes& boxes, Tiles& tiles, Lights& lights) {
	
	player.readFromBinary(fin);

	boxes.readFromBinary(fin);

	tiles.readFromBinary(fin);

	lights.readFromBinary(fin);

	fin.close();
}

