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
	void tempSetAll(const glm::vec4 &desRec, const Lengine::GLtexture& texture);
	void addToWorld(b2World* world);
	void update(Lengine::InputManager* inputmanager);
	void draw(Lengine::SpriteBatch* spritebatch,float deltaTime=1.0f);
	void tempDraw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender* debugrender);
	void tempDebugDraw(Lengine::DebugRender* debugrender,bool isSelected = false);
	Capsule& getCapsule(){ return m_capsule; }
	const Capsule& getConstCapsule() const { return m_capsule; }
	Lengine::ColorRGBA8 color;
	Lengine::GLtexture texture;
private:
	Capsule m_capsule;
};