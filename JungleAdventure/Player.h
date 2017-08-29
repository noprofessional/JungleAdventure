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
	void tempSetAll(const glm::vec4 &RenderDesRec, const glm::vec2& CollisionDim, const glm::vec2& PosOffset, const Lengine::GLtexture& texture);
	void addToWorld(b2World* world);
	void update(Lengine::InputManager* inputmanager);
	void draw(Lengine::SpriteBatch* spritebatch, float deltaTime = 1.0f);
	void tempDraw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender* debugrender);
	void tempDebugDraw(Lengine::DebugRender* debugrender, bool isSelected = false);

	//////////////////////////////////////////////////////////////////////////
	//getters
	//////////////////////////////////////////////////////////////////////////
	const glm::vec2&			getTempPos()const		{ return m_tempPos; }
	glm::vec2&					getTempPosReference()	{ return m_tempPos; }
	const glm::vec2&			getRenderDim()const		{ return m_renderDim; }
	const glm::vec2&			getCollisionDim()const	{ return m_collisoinDim; }
	const glm::vec2&			getPosOffset()const		{ return m_posOffset; }
	const Lengine::ColorRGBA8&	getColor()const			{ return m_color; }
	const Lengine::GLtexture&	getTexture()const		{ return m_texture; }
	Capsule&					getCapsule()			{ return m_capsule; }
	const Capsule&				getConstCapsule()const	{ return m_capsule; }
private:

	glm::vec2 m_tempPos;
	glm::vec2 m_renderDim;
	glm::vec2 m_collisoinDim;

	//=collision POS - render POS
	glm::vec2 m_posOffset;

	Lengine::ColorRGBA8 m_color;
	Lengine::GLtexture m_texture;
	Capsule m_capsule;
};