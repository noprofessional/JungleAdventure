#include "LevelWriterNReader.h"

#include<fstream>
#include<Lengine/FatalError.h>
#include<Lengine/TextureCache.h>

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

	//temp val for easy use
	const Lengine::ColorRGBA8* Col;

	//player
	Col= &player.getColor();
	fileOut << player.getTempPos().x << ' ' << player.getTempPos().y << ' '
		<< player.getRenderDim().x << ' ' << player.getRenderDim().y << ' '
		<< player.getCollisionDim().x << ' ' << player.getCollisionDim().y << ' '
		<< player.getPosOffset().x << ' ' << player.getPosOffset().y << ' '
		<< player.getTexture()->filePath << '\n';

	//boxes
	fileOut << boxes.size() << '\n';
	for (auto& B : boxes) {
		Col = &(B.color);
		fileOut << B.tempPos.x << ' ' << B.tempPos.y << ' '
			<< B.dimension.x << ' ' << B.dimension.y << ' '
			<< B.tempAngle << ' '
			<<B.physicMode<<' '
			<< Col->r << ' ' << Col->g << ' ' << Col->b << ' ' << Col->a << ' '
			<< B.texture->filePath << '\n';
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
	
	std::string texturePath;

	//player load
	{
		glm::vec4 desRec;
		glm::vec2 collisionDim;
		glm::vec2 posOffset;
		fileIn >> desRec.x >> desRec.y >> desRec.z >> desRec.w 
			>>collisionDim.x>>collisionDim.y>>posOffset.x>>posOffset.y
			>> texturePath;
		player.tempSetAll(desRec, collisionDim,posOffset, Lengine::textureCache.gettexture(texturePath));
	}
	//boxes load
	{
		glm::vec4 desRec;
		float angle;
		int physicModeCode;
		PhysicMode physicMode;
		Lengine::ColorRGBA8 color;

		int size;
		fileIn >> size;
		boxes.resize(size);

		for (int i = 0;i < size;i++) {
			fileIn >> desRec.x >> desRec.y >> desRec.z >> desRec.w >> angle >> physicModeCode
				>> color.r >> color.g >> color.b >> color.a >> texturePath;
			switch (physicModeCode)
			{
			case PhysicMode::DYNAMIC :
				physicMode = PhysicMode::DYNAMIC;
				break;
			case PhysicMode::RIGID :
				physicMode = PhysicMode::RIGID;
				break;
			case PhysicMode::MOVABLE :
				physicMode = PhysicMode::MOVABLE;
			case PhysicMode::VOIDSPACE :
				physicMode = PhysicMode::VOIDSPACE;
				break;
			}
			boxes[i].tempSetAll(desRec, angle, color, Lengine::textureCache.gettexture(texturePath), physicMode);
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