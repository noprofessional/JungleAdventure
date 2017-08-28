#include "LevelWriterNReader.h"

#include<fstream>
#include<Lengine/FatalError.h>
#include<Lengine/ResourceManager.h>

#include"Player.h"
#include"Box.h"
#include"Light.h"


// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;


void LevelWriterNReader::saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	saveTextV0(filePath, player, boxes, lights);
}
void LevelWriterNReader::saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	Lengine::fatalError("save binary is not implement");
}
void LevelWriterNReader::readAsText(const std::string& filePath, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	std::ifstream fileIn(filePath);
	if (fileIn.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("File " + filePath + " can not be open!");
	}
	unsigned int version;
	fileIn >> version;
	switch (version) {
	case TEXT_VERSION_0:
		readTextV0(fileIn, player, boxes, lights);
		break;
//	case otherVersion:
//		readTextV...;
//		break;
	default:
		Lengine::fatalError("Unknow version in leverl file!\nFile may have been corrupted...");
	}
}
void LevelWriterNReader::readAsBinary(const std::string& filePath, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	Lengine::fatalError("read binary is not implement");
}

void LevelWriterNReader::saveTextV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
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
	
	const Lengine::ColorRGBA8* Col = &player.getColor();
	fileOut << player.getTempPos().x << ' '<< player.getTempPos().y << ' '
		<< player.getRenderDim().x << ' ' << player.getRenderDim().y << ' '
		<< player.getTexture().filePath << '\n';
	//boxes
	fileOut << boxes.size() << '\n';
	for (auto& B : boxes) {
		Col = &(B.color);
		fileOut << B.tempPos.x << ' ' << B.tempPos.y << ' '
			<< B.dimension.x << ' ' << B.dimension.y << ' '
			<< B.tempAngle << ' '
			<<int(B.physicMode)<<' '
			<< Col->r << ' ' << Col->g << ' ' << Col->b << ' ' << Col->a << ' '
			<< B.texture.filePath << '\n';
	}
	//lights
	fileOut << lights.size() << '\n';
	for (auto& L : lights) {
		Col = &(L.color);
		fileOut << L.centerPos.x << ' ' << L.centerPos.y << ' '
			<< L.size << ' '
			<< Col->r << Col->g << Col->b << Col->a << '\n';
	}

}
	
void LevelWriterNReader::readTextV0(std::ifstream& fileIn, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights) {
	//player load
	{
		glm::vec4 desRec;
		Lengine::GLtexture texture;
		fileIn >> desRec.x >> desRec.y >> desRec.z >> desRec.w >> texture.filePath;
		texture = Lengine::ResourceManager::gettexture(texture.filePath);
		player.tempSetAll(desRec, texture);
	}
	//boxes load
	{
		glm::vec4 desRec;
		float angle;
		int physicModeCode;
		PhysicMode physicMode;
		Lengine::ColorRGBA8 color;
		Lengine::GLtexture texture;

		int size;
		fileIn >> size;
		boxes.resize(size);

		for (int i = 0;i < size;i++) {
			fileIn >> desRec.x >> desRec.y >> desRec.z >> desRec.w >> angle >> physicModeCode
				>> color.r >> color.g >> color.b >> color.a >> texture.filePath;
			switch (physicModeCode)
			{
			case int(PhysicMode::DYNAMIC) :
				physicMode = PhysicMode::DYNAMIC;
				break;
			case int(PhysicMode::RIGID) :
				physicMode = PhysicMode::RIGID;
				break;
			case int(PhysicMode::MOVABLE) :
				physicMode = PhysicMode::MOVABLE;
				break;
			}
			texture = Lengine::ResourceManager::gettexture(texture.filePath);
			boxes[i].tempSetAll(desRec, angle, color, texture, physicMode);
		}
	}
	//lights load
	{
		int size;
		fileIn >> size;
		lights.resize(size);
		for (int i = 0;i < size;i++) {
			fileIn >> lights[i].centerPos.x >> lights[i].centerPos.y >> lights[i].size
				>> lights[i].color.r >> lights[i].color.g >> lights[i].color.b >> lights[i].color.a;
		}
	}
	fileIn.close();
}