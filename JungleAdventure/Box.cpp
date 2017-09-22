#include "Box.h"
#include <iostream>
#include <random>
#include <ctime>
#include <fstream>

Box::Box() {
}
Box::~Box() {
	//don't need to destroy body which will be destroied after the world is expired
}

void Box::tempSetAll(
		const glm::vec4& desRec,
		const glm::vec4& UVRect,
		const float& angle,
		const float& Depth,
		const Lengine::ColorRGBA8& Color,
		Lengine::GLtexture* Texture, 
		const PhysicMode& PhysicMode) 
{
	tempPos = glm::vec2(desRec.x, desRec.y);
	dimension = glm::vec2(desRec.z, desRec.w);
	uvRect = UVRect;
	tempAngle = angle;
	depth = Depth;
	color = Color;
	texture = Texture;
	physicMode = PhysicMode;
}

void Box::addToWorld(b2World* world) {
	b2BodyDef bodydef;
	switch (physicMode)
	{
	case PhysicMode::RIGID:
		bodydef.type = b2_staticBody;
		break;
	case PhysicMode::DYNAMIC:
		bodydef.type = b2_dynamicBody;
		break;
	case PhysicMode::MOVABLE:
		bodydef.type = b2_kinematicBody;
		break;
	case PhysicMode::VOIDSPACE:
		isAddedToWorld = false;
		break;
	}
	//get the body only if it is designed to add to the 2DBox World
	if (isAddedToWorld) {
		bodydef.position.Set(tempPos.x, tempPos.y);
		bodydef.angle = tempAngle;
		m_body = world->CreateBody(&bodydef);

		b2PolygonShape shape;
		shape.SetAsBox(dimension.x/2.0f, dimension.y/2.0f);
		b2Fixture* fixture = m_body->CreateFixture(&shape, 1.0f);
	}
}

void Box::tempDebugDraw(Lengine::DebugRender* debugRenderer, bool selected /*= false*/) {
	if (!selected) {
		switch (physicMode)
		{
		case PhysicMode::RIGID:
			debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(255, 0, 0, 255), tempAngle);
			break;
		case PhysicMode::DYNAMIC:
			debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(0, 255, 0, 255), tempAngle);
			break;
		case PhysicMode::MOVABLE:
			debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(255, 0, 255, 255), tempAngle);
			break;
		case PhysicMode::VOIDSPACE:
			debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(255, 255, 255, 255), tempAngle);
		}
	}
	else {
		debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(255, 255, 0, 255), tempAngle);
	}
}

void Box::draw(Lengine::SpriteBatch* spritebatch) {
	texture = Lengine::textureCache.gettexture(texture->filePath);
	const glm::vec4 uvrec(0.0f, 0.0f, dimension.x, dimension.y);
	spritebatch->draw(getRenderDesRec(), uvrec, texture->ids[0], depth, color,getRenderAngle());
}
void Box::clampedDraw(Lengine::SpriteBatch* spritebatch) {
	float x = dimension.x;
	float y = dimension.y;
	glm::vec4 leftBottomDesRec(getRenderDesRec().x - dimension.x / 2.0f + 0.5f, getRenderDesRec().y - dimension.y / 2.0f + 0.5f, 1.0f, 1.0f);
	texture = Lengine::textureCache.getSTClampedTexture(texture->filePath);

	for (float x = 0.0f;x < dimension.x;x++) {
		for (float y = 0.0f;y < dimension.y;y++) {
			spritebatch->draw(leftBottomDesRec + glm::vec4(x, y, 0.0f, 0.0f), uvRect, texture->ids[0], depth, color, getRenderAngle());
		}
	}
}

void Box::debugDraw(Lengine::DebugRender * debugRenderer) {
	if (isAddedToWorld) {
		debugRenderer->drawBox(getRenderDesRec(), Lengine::ColorRGBA8(255, 255, 255, 255), getRenderAngle());
	}
}

bool Box::isInBox(const glm::vec2 &pos) {
	glm::vec2 desVec = pos - tempPos;
	if (abs(desVec.x) < dimension.x/2.0f&& abs(desVec.y) < dimension.y/2.0f)
		return true;
	else
		return false;
}

void Box::writeAsBinary(std::ofstream& fout) const {
	fout.write((char*)&tempPos, sizeof(glm::vec2));
	fout.write((char*)&dimension, sizeof(glm::vec2));
	fout.write((char*)&uvRect, sizeof(glm::vec4));
	fout.write((char*)&tempAngle, sizeof(float));
	fout.write((char*)&depth, sizeof(float));
	fout.write((char*)&color, sizeof(Lengine::ColorRGBA8));

	const char * str = texture->filePath.c_str();
	int len = strlen(str) + 1;
	fout.write((char*)&len, sizeof(int));
	fout.write(str, len);

	fout.write((char*)&physicMode, sizeof(physicMode));

}
void Box::readFromBinary(std::ifstream& fin) {
	fin.read((char*)&tempPos, sizeof(glm::vec2));
	fin.read((char*)&dimension, sizeof(glm::vec2));
	fin.read((char*)&uvRect, sizeof(glm::vec4));
	fin.read((char*)&tempAngle, sizeof(float));
	fin.read((char*)&depth, sizeof(float));
	fin.read((char*)&color, sizeof(Lengine::ColorRGBA8));

	int len = 0;
	char str[500];
	fin.read((char*)&len, sizeof(int));
	fin.read(str, len);

	fin.read((char*)&physicMode, sizeof(physicMode));
}

glm::vec4 Box::getRenderDesRec()  {
	glm::vec2 Pos = tempPos;
	//if it has been added to the world
	if (m_body) {
		Pos = glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y);
	}
	glm::vec4 DesRec(Pos, dimension);
	return DesRec;
}

float Box::getRenderAngle() {
	if (m_body)
		return m_body->GetAngle();
	else
		return tempAngle;
}