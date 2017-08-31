#pragma once
#include<Box2D/Box2D.h>
#include<glm/glm.hpp>
#include<Lengine/TextureCache.h>
#include <Lengine/SpriteBatch.h>
#include<Lengine/Vertex.h>
#include<Lengine/DebugRender.h>
enum class PhysicMode {
	RIGID,
	DYNAMIC,
	MOVABLE
};
class Box
{
public:
	Box();
	~Box();
	void addToWorld(b2World* world);
	void draw(Lengine::SpriteBatch * spritebatch);
	void debugDraw(Lengine::DebugRender * spritebatch);
	void destroy();

	//before added to the world
	void tempSetAll(const glm::vec4& desRec,const float& tempAngle,
		const Lengine::ColorRGBA8& color,const Lengine::GLtexture& texture,const PhysicMode& physicMode);
	void tempDraw(Lengine::SpriteBatch * spritebatch);
	void tempDebugDraw(Lengine::DebugRender* debugRenderer,bool selected= false);
	//-------- operator --------
	//Box& operator=(Box&src);
	//-------- getter --------
	//get the position after added to the world
	glm::vec2 getpos()const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	//get the angle after added to the world
	float getangle() const{ return m_body->GetAngle(); }
	//get the body after added to the world
	b2Body* getbody() { return m_body; }
	bool isInBox(const glm::vec2 pos);

	glm::vec2 tempPos;
	glm::vec2 dimension;
	Lengine::ColorRGBA8 color;
	Lengine::GLtexture texture;
	float tempAngle;
	PhysicMode physicMode;
private:
	b2Body * m_body = nullptr;
};

