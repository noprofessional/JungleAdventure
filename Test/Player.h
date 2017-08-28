#pragma once
#include "Capsule.h"
#include<Lengine/TextureCache.h>
#include<Lengine/InputManager.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/DebugRender.h>
class Player {
public:
	Player();
	~Player();
	void tempSetAll(const glm::vec4 &RenderDesRec, const Lengine::GLtexture& texture);
	void addToWorld(b2World* world,const glm::vec2& CollisionDim,glm::vec2 PosOffset);
	void update(Lengine::InputManager* inputmanager);
	void draw(Lengine::SpriteBatch* spritebatch,float deltaTime=1.0f);
	void tempDraw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender* debugrender);
	void tempDebugDraw(Lengine::DebugRender* debugrender,bool isSelected = false);

	//////////////////////////////////////////////////////////////////////////
	//getters
	//////////////////////////////////////////////////////////////////////////
	const glm::vec2&			getTempPos()const		{ return m_tempPos; }
	const glm::vec2&			getRenderDim()const		{ return m_renderDim; }
	const Lengine::ColorRGBA8&	getColor()const			{ return m_color; }
	const Lengine::GLtexture&	getTexture()const		{ return m_texture; }
	Capsule& getCapsule(){ return m_capsule; }
	const Capsule& getConstCapsule() const { return m_capsule; }
private:
	glm::vec2 m_tempPos;
	glm::vec2 m_renderPosOffset;
	glm::vec2 m_renderDim;
	Lengine::ColorRGBA8 m_color;
	Lengine::GLtexture m_texture;
	Capsule m_capsule;
};