#include "LevelWriterNReader.h"

#include<fstream>
#include<Lengine/FatalError.h>
#include<Lengine/TextureCache.h>

#include"Player.h"
#include"Box.h"
#include"Light.h"
#include <Lengine/SpriteBatch.h>

// When you want to make a new version, add it here
const unsigned int TEXT_VERSION_0 = 100;
const unsigned int BIN_VERSION_0 = 100;

// Make sure this is set to the current version
const unsigned int TEXT_VERSION = TEXT_VERSION_0;
const unsigned int BIN_VERSION = BIN_VERSION_0;

void LevelWriterNReader::saveAsText(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights) {
	saveTextV0(filePath, player, boxes, lights);
}
void LevelWriterNReader::saveAsBinary(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights, const std::vector<unsigned char>& frameRawData, const int& width, const int& height) {
	saveAsBinaryV0(filePath, player, boxes, lights, frameRawData,width,height);
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
void LevelWriterNReader::readAsBinary(const std::string& filePath, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights, std::vector<unsigned char>& frameRawData, int& width, int& height) {
	
	std::ifstream fileIn(filePath,std::ios_base::binary);
	if (fileIn.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("File " + filePath + " can not be open!");
	}

	unsigned int version;
	fileIn.read((char*)&version, sizeof(unsigned int));

	switch (version) {
	case BIN_VERSION_0:
		readAsBinaryV0(fileIn, player, boxes, lights,frameRawData,width,height);
		break;
//	case otherVersion:
//		readTextV...;
//		break;
	default:
		Lengine::fatalError("Unknow version in leverl file!\nFile may have been corrupted...");
	}
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
		fileOut
			<< B.tempPos.x << ' ' << B.tempPos.y << ' ' << B.dimension.x << ' ' << B.dimension.y << ' '
			<< B.uvRect.x << ' ' << B.uvRect.y << ' ' << B.uvRect.z << ' ' << B.uvRect.w << ' '
			<< B.tempAngle << ' '
			<< B.depth << ' '
			<< B.physicMode << ' '
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
void LevelWriterNReader::saveAsBinaryV0(const std::string& filePath, const Player& player, const std::vector<Box>& boxes, const std::vector<Light>& lights, const std::vector<unsigned char>& frameRawData,  const int& width, const int& height) {
	
	std::ofstream fout(filePath.c_str(), std::ios_base::binary);
	if (fout.fail()) {
		perror(filePath.c_str());
		Lengine::fatalError("file " + filePath + " open failed!\n");
	}

	fout.write((char*)&BIN_VERSION, sizeof(unsigned int));

	fout.write((char*)&width, sizeof(width));
	fout.write((char*)&height, sizeof(height));
	fout.write((char*)frameRawData.data(), frameRawData.size());

	player.writeAsBinary(fout);

	int size = boxes.size();
	fout.write((char*)&size, sizeof(int));
	for (auto& B : boxes) {
		B.writeAsBinary(fout);
	}

	size = lights.size();
	fout.write((char*)&size, sizeof(int));
	for (auto& L : lights) {
		L.writeAsBinary(fout);
	}

	fout.close();
	
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
		player.tempSetAll(desRec, collisionDim,posOffset, Lengine::textureCache.getSTClampedTexture(texturePath));
	}
	//boxes load
	{
		glm::vec4 desRec;
		glm::vec4 uvRect;
		float angle;
		float depth;
		int physicModeCode;
		PhysicMode physicMode;
		Lengine::ColorRGBA8 color;

		int size;
		fileIn >> size;
		boxes.resize(size);

		for (int i = 0;i < size;i++) {
			fileIn >> desRec.x >> desRec.y >> desRec.z >> desRec.w
				>>uvRect.x>> uvRect.y >> uvRect.z >> uvRect.w
				>> angle >> depth
				>> physicModeCode
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
			boxes[i].tempSetAll(desRec,uvRect, angle, depth, color, Lengine::textureCache.gettexture(texturePath), physicMode);
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
void LevelWriterNReader::readAsBinaryV0(std::ifstream& fin, Player& player, std::vector<Box>& boxes, std::vector<Light>& lights, std::vector<unsigned char>& frameRawData, int& width, int& height) {
	fin.read((char*)&width, sizeof(int));
	fin.read((char*)&height, sizeof(int));

	frameRawData.clear();
	frameRawData.resize(width * height * 4);
	fin.read((char*)frameRawData.data(), frameRawData.size());

	player.readFromBinary(fin);

	int size = 0;
	fin.read((char*)&size, sizeof(int));
	boxes.clear();
	boxes.resize(size);
	for (auto& B : boxes) {
		B.readFromBinary(fin);
	}

	fin.read((char*)&size, sizeof(int));
	lights.clear();
	lights.resize(size);
	for (auto& L : lights) {
		L.readFromBinary(fin);
	}

	fin.close();
}

