#include "Capsule.h"

Capsule::Capsule() {

}

Capsule::Capsule(const glm::vec4& desRec, float angle, bool fixedRotation) {
	m_desRec = desRec;
	m_angle = angle;
	m_fixedRotation = fixedRotation;
	m_body = nullptr;
}

Capsule::~Capsule() {

}

void Capsule::addToWorld(b2World* world) {
	//create body
	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(m_desRec.x, m_desRec.y);
	bodydef.angle = m_angle;
	bodydef.fixedRotation = m_fixedRotation;
	m_body = world->CreateBody(&bodydef);

	//create box
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(m_desRec.z / 2.0f, (m_desRec.w - m_desRec.z) / 2.0f);
	b2FixtureDef fixtruedef;
	fixtruedef.shape = &polygonShape;
	fixtruedef.density = 1.0f;
	fixtruedef.friction = 0.5f;
	m_fixtures[0] = m_body->CreateFixture(&fixtruedef);

	//create circles
	b2CircleShape circleShape;
	circleShape.m_radius = (m_desRec.z + 0.1f) / 2.0f;

	b2FixtureDef circleFixtureDef;
	circleFixtureDef.shape = &circleShape;
	circleFixtureDef.density = 0.5f;
	circleFixtureDef.friction = 0.2f;
	//bottom circle
	circleShape.m_p.Set(0.0f, -(m_desRec.w - m_desRec.z) / 2.0f);
	m_fixtures[1] = m_body->CreateFixture(&circleFixtureDef);
	//top circle
	circleShape.m_p.Set(0.0f, (m_desRec.w - m_desRec.z) / 2.0f);
	m_fixtures[2] = m_body->CreateFixture(&circleFixtureDef);
}

bool Capsule::inCapsule(const glm::vec2& mouseCords) {
	//only for edit screen
	glm::vec2 desVec = mouseCords - glm::vec2(m_desRec.x, m_desRec.y);
	if (abs(desVec.x) < m_desRec.z / 2.0f&&abs(desVec.y) < m_desRec.w)
		return true;
	else
		return false;
}

void Capsule::debugdraw(Lengine::DebugRender* debugrender,bool isSelected) {

	if (m_body) {
		m_desRec.x = m_body->GetPosition().x;
		m_desRec.y = m_body->GetPosition().y;
		m_angle = m_body->GetAngle();
	}
	if (isSelected) {
		debugrender->drawCapsule(m_desRec, Lengine::YELLOW, m_angle);
	}
	else {
		debugrender->drawCapsule(m_desRec, Lengine::WHITE, m_angle);
	}
	
}
