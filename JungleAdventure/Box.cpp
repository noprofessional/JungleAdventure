#include "Box.h"
#include<iostream>
#include<random>
#include<ctime>

Box::Box() {
}
Box::~Box() {
	//don't need to destroy body which will be destroied after the world is expired
}

void Box::tempSetAll(const glm::vec4& desRec,
	const float& angle,
	const Lengine::ColorRGBA8& Color,
	Lengine::GLtexture* Texture, 
	const PhysicMode& PhysicMode) {
	tempPos = glm::vec2(desRec.x, desRec.y);
	dimension = glm::vec2(desRec.z, desRec.w);
	tempAngle = angle;
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

void Box::tempDraw(Lengine::SpriteBatch * spritebatch) {
	spritebatch->draw(getRenderDesRec(), glm::vec4(0.0f, 0.0f, dimension.x, dimension.y), texture->ids[0], 1.0f, color, tempAngle);
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
	const glm::vec4 uvrec(0.0f, 0.0f, dimension.x, dimension.y);
	spritebatch->draw(getRenderDesRec(), uvrec, texture->ids[0], 1.0f, color,getRenderAngle());
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

glm::vec4 Box::getRenderDesRec() {
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