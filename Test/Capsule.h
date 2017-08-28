#pragma once
#include<Box2D/Box2D.h>
#include<glm/glm.hpp>
#include<Lengine/DebugRender.h>
class Capsule
{
public:
	void init(b2World* world, const glm::vec4& posrec, float angle, bool fixedrotation);

	void debugdraw(Lengine::DebugRender* debugrender);

	glm::vec2 getpos()const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	const glm::vec2& getDim() const { return m_dimension; }
	float getangle() const { return m_body->GetAngle(); }
	b2Body* getbody() { return m_body; }

private:
	b2Body* m_body;
	b2Fixture* m_fixtures[3];
	glm::vec2 m_dimension;
};

