#pragma once
#include<Box2D/Box2D.h>
#include<glm/glm.hpp>
#include<Lengine/DebugRender.h>
class Capsule
{
public:
	Capsule();
	Capsule(const glm::vec4& desRec, float angle, bool fixedRotation);
	~Capsule();

	void addToWorld(b2World* world);
	bool inCapsule(const glm::vec2& mouseCords);
	void debugdraw(Lengine::DebugRender* debugrender,bool isSelected);

	glm::vec2			getpos()const	{ return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	float				getangle()const { return m_body->GetAngle(); }
	b2Body*				getbody()		{ return m_body; }

private:

	glm::vec4 m_desRec;
	float m_angle;
	bool m_fixedRotation;

	b2Body* m_body;
	b2Fixture* m_fixtures[3];
};

