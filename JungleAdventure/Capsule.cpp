#include "Capsule.h"



void Capsule::tempSetAll(const glm::vec4& desRec, float tempAngle) {
	tempPos = glm::vec2(desRec.x, desRec.y);
	dimension = glm::vec2(desRec.z, desRec.w);
	tempAngle = tempAngle;
}
void Capsule::addToWorld(b2World* world, bool fixedrotation) {
	//create body
	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(tempPos.x, tempPos.y);
	bodydef.angle = tempAngle;
	bodydef.fixedRotation = fixedrotation;
	m_body = world->CreateBody(&bodydef);

	//create box
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(dimension.x / 2.0f, (dimension.y - dimension.x) / 2.0f);
	b2FixtureDef fixtruedef;
	fixtruedef.shape = &polygonShape;
	fixtruedef.density = 1.0f;
	fixtruedef.friction = 0.5f;
	m_fixtures[0] = m_body->CreateFixture(&fixtruedef);

	//create circles
	b2CircleShape circleShape;
	circleShape.m_radius = dimension.x / 2.0f;

	b2FixtureDef circleFixtureDef;
	circleFixtureDef.shape = &circleShape;
	circleFixtureDef.density = 0.5f;
	circleFixtureDef.friction = 0.01f;
	//bottom circle
	circleShape.m_p.Set(0.0f, -(dimension.y - dimension.x) / 2.0f);
	m_fixtures[1] = m_body->CreateFixture(&circleFixtureDef);
	//top circle
	circleShape.m_p.Set(0.0f, (dimension.y - dimension.x) / 2.0f);
	m_fixtures[2] = m_body->CreateFixture(&circleFixtureDef);
}

void Capsule::debugdraw(Lengine::DebugRender* debugrender) {
	glm::vec4 desRec;
	desRec.x = m_body->GetPosition().x;
	desRec.y = m_body->GetPosition().y;
	desRec.z = dimension.x;
	desRec.w = dimension.y;
	debugrender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 255, 255), tempAngle);
}
void Capsule::tempDebugDraw(Lengine::DebugRender* debugrender, bool isSelected/* = false*/) {
	glm::vec4 desRec(tempPos, dimension);
	if (isSelected) {
		debugrender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 0, 255), tempAngle);
	}
	else {
		debugrender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 255, 255), tempAngle);
	}
}
