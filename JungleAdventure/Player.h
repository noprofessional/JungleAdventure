#pragma once
#include "Capsule.h"
#include<Lengine/TextureCache.h>
#include<Lengine/InputManager.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/DebugRender.h>


class Player {
public:
	Player();
	Player(const glm::vec4 &RenderDesRec, const glm::vec2& CollisionDim, const glm::vec2& PosOffset);
	~Player();

	void addToWorld(b2World* world);
	
	void update(Lengine::InputManager* inputmanager, float deltaTime = 1.0f);
	
	void startContact(b2Contact* contact);
	void endContact(b2Contact* contact);

	void draw(Lengine::SpriteBatch* spritebatch);
	void debugDraw(Lengine::DebugRender* debugrender);

	bool selectPlayer(const glm::vec2& mouseCords);

	void writeAsText(std::ofstream& fout)const;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const ;
	void readFromBinary(std::ifstream& fin);

	void changePos(const float& xOffset, const float & yOffset);
	glm::vec2 getPos() { return m_capsule.getpos(); }
	glm::vec2 getMove();

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

	void init();
	glm::vec4 getRenderDesRect();

	glm::vec2 m_renderPos;
	glm::vec2 m_renderDim;
	glm::vec2 m_collisionDim;
	glm::vec2 m_posOffset;	//=collision POS - render POS

	glm::vec2 m_preCPos;	//previous collision position
	glm::vec2 m_move;

	Lengine::ColorRGBA8 m_color;
	Lengine::GLtexture* m_texture;
	int m_imageCounter=0;
	glm::vec4 m_UVdesRec;
	Lengine::GLtexture* m_textures[PlayerState::TOTAL];

	float m_limitSpeed;
	PlayerState m_playerState = PlayerState::IDLE;

	int m_contactNUM = 0;
	bool m_isOnGround = false;
	bool m_isPending = false;
	bool m_isSelected = false;
	bool m_isInit = false;

	Capsule m_capsule;
};