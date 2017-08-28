#pragma once
#include<Box2D/Box2D.h>
#include<glm/glm.hpp>
#include<Lengine/DebugRender.h>
class Capsule
{
public:
	void tempSetAll(const glm::vec4& posrec, float angle);
	void addToWorld(b2World* world, bool fixedrotation);

	void debugdraw(Lengine::DebugRender* debugrender);
	void tempDebugDraw(Lengine::DebugRender* debugrender,bool isSelected=false);

	glm::vec2 getpos()const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	float getangle() const { return m_body->GetAngle(); }
	b2Body* getbody() { return m_body; }

	glm::vec2 tempPos;
	glm::vec2 dimension;
	float tempAngle;

private:
	b2Body* m_body;
	b2Fixture* m_fixtures[3];
};

