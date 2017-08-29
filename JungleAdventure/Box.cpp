#include "Box.h"
#include<iostream>
#include<random>
#include<ctime>

Box::Box() {

}
Box::~Box() {

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
	}

	bodydef.position.Set(tempPos.x, tempPos.y);
	bodydef.angle = tempAngle;
	m_body = world->CreateBody(&bodydef);

	b2PolygonShape shape;
	shape.SetAsBox(dimension.x / 2.0f, dimension.y / 2.0f);
	b2Fixture* fixture = m_body->CreateFixture(&shape, 1.0f);
}
void Box::draw(Lengine::SpriteBatch* spritebatch) {
	glm::vec2 pos(m_body->GetPosition().x, m_body->GetPosition().y);
	glm::vec4 posrec(pos, dimension);
	const glm::vec4 uvrec(0.0f, 0.0f, dimension.x, dimension.y);
	
	spritebatch->draw(posrec, uvrec, texture.ids[0], 1.0f, color,m_body->GetAngle());

}
void Box::debugDraw(Lengine::DebugRender * debugRenderer) {
	glm::vec2 pos(m_body->GetPosition().x, m_body->GetPosition().y);
	glm::vec4 desRec(pos, dimension);
	const glm::vec4 uvrec(0.0f, 0.0f, dimension.x, dimension.y);
	debugRenderer->drawBox(desRec, Lengine::ColorRGBA8(255, 255, 255, 255), m_body->GetAngle());

}
void Box::destroy() {
	m_body->GetWorld()->DestroyBody(m_body);
	m_body = nullptr;
}

void Box::tempSetAll(const glm::vec4& desRec, const float& angle,
	const Lengine::ColorRGBA8& Color, const Lengine::GLtexture& Texture, const PhysicMode& PhysicMode) {
	tempPos = glm::vec2(desRec.x, desRec.y);
	dimension = glm::vec2(desRec.z, desRec.w);
	tempAngle = angle;
	color = Color;
	texture = Texture;
	physicMode = PhysicMode;
}
void Box::tempDraw(Lengine::SpriteBatch * spritebatch) {
	glm::vec4 tempDesRec(tempPos, dimension);
	spritebatch->draw(tempDesRec, glm::vec4(0.0f, 0.0f, dimension.x, dimension.y), texture.ids[0], 1.0f, color, tempAngle);
}
void Box::tempDebugDraw(Lengine::DebugRender* debugRenderer, bool selected /*= false*/) {
	glm::vec4 desRec(tempPos, dimension);
	if (!selected) {
		switch (physicMode)
		{
		case PhysicMode::RIGID:
			debugRenderer->drawBox(desRec, Lengine::ColorRGBA8(255, 0, 0, 255), tempAngle);
			break;
		case PhysicMode::DYNAMIC:
			debugRenderer->drawBox(desRec, Lengine::ColorRGBA8(0, 255, 0, 255), tempAngle);
			break;
		case PhysicMode::MOVABLE:
			debugRenderer->drawBox(desRec, Lengine::ColorRGBA8(255, 0, 255, 255), tempAngle);
			break;
		}
	}
	else {
		debugRenderer->drawBox(desRec, Lengine::ColorRGBA8(255, 255, 0, 255), tempAngle);
	}
}


Box& Box::operator=(Box&src) {
	tempPos = src.tempPos;
	dimension = src.dimension;
	color = src.color;
	texture = src.texture;
	tempAngle = src.tempAngle;
	return *this;
}
