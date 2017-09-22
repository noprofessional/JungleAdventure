#pragma once
#include<Box2D/Box2D.h>
#include<glm/glm.hpp>
#include<Lengine/TextureCache.h>
#include <Lengine/SpriteBatch.h>
#include<Lengine/Vertex.h>
#include<Lengine/DebugRender.h>
enum PhysicMode {
	RIGID,
	DYNAMIC,
	MOVABLE,
	VOIDSPACE
};
class Box
{
public:
	Box();
	~Box();
	//for edit screen use
	void tempSetAll(
		const glm::vec4& desRec,
		const glm::vec4& UVRect,
		const float& tempAngle,
		const float& depth,
		const Lengine::ColorRGBA8& color,
		Lengine::GLtexture* texture, 
		const PhysicMode& physicMode);

	//use in play screen
	void addToWorld(b2World* world);

	//temp draw
	void tempDebugDraw(Lengine::DebugRender* debugRenderer, bool selected = false);

	//draw with information in 2dWorld
	void draw(Lengine::SpriteBatch * spritebatch);
	void clampedDraw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender * spritebatch);

	//if pos is in box
	bool isInBox(const glm::vec2 &pos);

	void writeAsBinary(std::ofstream& fout)const ;
	void readFromBinary(std::ifstream& fin);

	//-------- getter --------
	//get the position after added to the world
	glm::vec2 getpos()const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
	//get the angle after added to the world
	float getangle() const{ return m_body->GetAngle(); }
	//get the body after added to the world
	b2Body* getbody() { return m_body; }

	glm::vec2 tempPos;
	glm::vec2 dimension;
	glm::vec4 uvRect;
	Lengine::ColorRGBA8 color;
	Lengine::GLtexture* texture=nullptr;
	float tempAngle;
	float depth;
	PhysicMode physicMode;
private:
	glm::vec4 getRenderDesRec() ;
	float getRenderAngle() ;

	b2Body * m_body = nullptr;		///not init in edit stage
	bool isAddedToWorld = true;		///default to true
};

