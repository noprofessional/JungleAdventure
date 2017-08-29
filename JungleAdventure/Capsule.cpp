#include "Capsule.h"


void Capsule::init(b2World* world, const glm::vec4& desRec, float angle, bool fixedrotation) {

	m_dimension = glm::vec2(desRec.z, desRec.w);

	//create body
	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(desRec.x, desRec.y);
	bodydef.angle = angle;
	bodydef.fixedRotation = fixedrotation;
	m_body = world->CreateBody(&bodydef);

	//create box
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(m_dimension.x / 2.0f, (m_dimension.y - m_dimension.x) / 2.0f);
	b2FixtureDef fixtruedef;
	fixtruedef.shape = &polygonShape;
	fixtruedef.density = 1.0f;
	fixtruedef.friction = 0.5f;
	m_fixtures[0] = m_body->CreateFixture(&fixtruedef);

	//create circles
	b2CircleShape circleShape;
	circleShape.m_radius = m_dimension.x / 2.0f;

	b2FixtureDef circleFixtureDef;
	circleFixtureDef.shape = &circleShape;
	circleFixtureDef.density = 0.5f;
	circleFixtureDef.friction = 0.01f;
	//bottom circle
	circleShape.m_p.Set(0.0f, -(m_dimension.y - m_dimension.x) / 2.0f);
	m_fixtures[1] = m_body->CreateFixture(&circleFixtureDef);
	//top circle
	circleShape.m_p.Set(0.0f, (m_dimension.y - m_dimension.x) / 2.0f);
	m_fixtures[2] = m_body->CreateFixture(&circleFixtureDef);
}

void Capsule::debugdraw(Lengine::DebugRender* debugrender) {
	glm::vec4 desRec;
	desRec.x = m_body->GetPosition().x;
	desRec.y = m_body->GetPosition().y;
	desRec.z = m_dimension.x;
	desRec.w = m_dimension.y;
	float angle = m_body->GetAngle();
	debugrender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 255, 255), angle);
}
