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

	void tempSetAll(const glm::vec4 &RenderDesRec, const glm::vec2& CollisionDim, const glm::vec2& PosOffset, Lengine::GLtexture* texture);
	void addToWorld(b2World* world);
	
	void update(Lengine::InputManager* inputmanager, float deltaTime = 1.0f);
	
	void startContact(b2Contact* contact);
	void endContact(b2Contact* contact);

	void draw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender* debugrender);

	void tempDraw(Lengine::SpriteBatch* spritebatch);
	void tempDebugDraw(Lengine::DebugRender* debugrender, bool isSelected = false);

	bool isInPlayer(const glm::vec2& pos);

	void writeAsBinary(std::ofstream& fout)const ;
	void readFromBinary(std::ifstream& fin);


	//getters
	const glm::vec2&			getTempPos()const		{ return m_tempPos; }
	glm::vec2&					getTempPosReference()	{ return m_tempPos; }
	const glm::vec2&			getRenderDim()const		{ return m_renderDim; }
	const glm::vec2&			getCollisionDim()const	{ return m_collisoinDim; }
	const glm::vec2&			getPosOffset()const		{ return m_posOffset; }
	const Lengine::ColorRGBA8&	getColor()const			{ return m_color; }
	Lengine::GLtexture*			getTexture()const		{ return m_texture; }
	Capsule&					getCapsule()			{ return m_capsule; }


private:
	//only use in player
	enum PlayerState {
		IDLE,
		JUMP_UP,
		JUMP_MIDAIR,
		JUMP_DOWN,
		JUMP_LAND,
		WALK,
		RUN,
		DUCK,
		SLIDE,
		TOTAL
	};

	glm::vec4 getRenderDesRect();
	glm::vec4 getCollisionDesRect();

	glm::vec2 m_tempPos;
	glm::vec2 m_renderDim;
	glm::vec2 m_collisoinDim;
	glm::vec2 m_posOffset;	//=collision POS - render POS
	Lengine::ColorRGBA8 m_color;
	Lengine::GLtexture* m_texture=nullptr;
	int m_imageCounter=0;
	glm::vec4 m_UVdesRec= glm::vec4(0.0f,0.0f,1.0f,1.0f);
	Lengine::GLtexture* m_textures[PlayerState::TOTAL];
	PlayerState m_playerState = PlayerState::IDLE;

	int m_contactNUM = 0;
	bool m_isOnGround = true;
	bool m_isPending = false;

	const float MAX_WALK_SPEED = 5.0f;
	const float MAX_RUN_SPEED = 10.0f;
	const float MIN_SPEED = 0.1f;
	float limitSpeed = MAX_WALK_SPEED;

	Capsule m_capsule;
};