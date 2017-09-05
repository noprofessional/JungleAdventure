#include"Player.h"
#include<algorithm>

Player::Player() {

}

Player::~Player() {

}

void Player::tempSetAll(const glm::vec4 &RenderDesRec,
	const glm::vec2& CollisionDim,
	const glm::vec2& PosOffset, 
	Lengine::GLtexture* Texture) {
	m_tempPos = glm::vec2(RenderDesRec.x, RenderDesRec.y);
	m_renderDim = glm::vec2(RenderDesRec.z, RenderDesRec.w);
	m_collisoinDim = CollisionDim;
	m_posOffset = PosOffset;
	m_texture = Texture;

	m_color = Lengine::ColorRGBA8(255, 255, 255, 255);
}

void Player::addToWorld(b2World* world) {

	glm::vec4 CollisionDesRec(m_tempPos + m_posOffset, m_collisoinDim);
	m_capsule.init(world, CollisionDesRec, 0.0f, true);
	//add the player to the user data in Box2D world
	m_capsule.getbody()->SetUserData(this);

	//get all texture pointer
	m_textures[IDLE] =  Lengine::textureCache.gettexture("Textures/Player/IDLE.gif");
	m_textures[JUMP_UP] =  Lengine::textureCache.gettexture("Textures/Player/JUMP_1_UP.gif");
	m_textures[JUMP_MIDAIR] =  Lengine::textureCache.gettexture("Textures/Player/JUMP_2_MIDAIR.gif");
	m_textures[JUMP_DOWN] =  Lengine::textureCache.gettexture("Textures/Player/JUMP_3_DOWN.gif");
	m_textures[JUMP_LAND] =  Lengine::textureCache.gettexture("Textures/Player/JUMP_4_LAND.gif");
	m_textures[WALK] =  Lengine::textureCache.gettexture("Textures/Player/WALK.gif");
	m_textures[RUN] =  Lengine::textureCache.gettexture("Textures/Player/RUN.gif");
	m_textures[DUCK] =  Lengine::textureCache.gettexture("Textures/Player/DUCK.gif");
	m_textures[SLIDE] =  Lengine::textureCache.gettexture("Textures/Player/SLIDE.gif");

}

void Player::update(Lengine::InputManager* inputmanager, float deltaTime/*=1.0f*/) {
	const float MAX_SPEED = 5.0f;
	const float MIN_SPEED = 0.1f;

	b2Body* temp = m_capsule.getbody();

	//movement on X-axis
	if (inputmanager->isKEYdown(SDLK_a)) {
		temp->ApplyForceToCenter(b2Vec2(-6.0f, 0.0f), true);
	}
	else if (inputmanager->isKEYdown(SDLK_d)) {
		temp->ApplyForceToCenter(b2Vec2(6.0f, 0.0f), true);
	}
	else if (abs(temp->GetLinearVelocity().x) >= MIN_SPEED) {
		temp->SetLinearVelocity(b2Vec2(0.95*temp->GetLinearVelocity().x, temp->GetLinearVelocity().y));
	}
	else if (abs(temp->GetLinearVelocity().x) < MIN_SPEED) {
		temp->SetLinearVelocity(b2Vec2(0.0f, temp->GetLinearVelocity().y));
	}

	//limit the X-axised move speed
	if (temp->GetLinearVelocity().x > MAX_SPEED) {
		temp->SetLinearVelocity(b2Vec2(MAX_SPEED, temp->GetLinearVelocity().y));
	}
	else if (temp->GetLinearVelocity().x < -MAX_SPEED) {
		temp->SetLinearVelocity(b2Vec2(-MAX_SPEED, temp->GetLinearVelocity().y));
	}
	
	//movement on Y-axis
	if (m_isOnGround) {
		if (inputmanager->isKEYpressed(SDLK_w)) {
			m_playerState = DUCK;
			m_isPending = true;
		}
	}
	
	//update player state
	b2Vec2 velocity = temp->GetLinearVelocity();

	//state decide
	if (!m_isPending) {
		if (m_playerState == DUCK) {
			temp->ApplyLinearImpulse(b2Vec2(0.0f, 15.0f), b2Vec2(temp->GetPosition()), true);
			m_playerState = JUMP_UP;
		}
		else if (m_playerState == JUMP_MIDAIR) {
			m_playerState = JUMP_DOWN;
		}
		else if (m_playerState == JUMP_LAND) {
			m_playerState = IDLE;
		}
		else if (m_playerState == JUMP_UP) {
			if (velocity.y > 0) {
				m_playerState = JUMP_UP;
			}
			else {
				m_playerState = JUMP_MIDAIR;
				m_isPending = true;
			}
		}
		else if (m_playerState == JUMP_DOWN) {
			if (!m_isOnGround) {
				m_playerState = JUMP_DOWN;
			}
			else {
				m_playerState = JUMP_LAND;
				m_isPending = true;
			}
		}
		else {
			if (m_isOnGround) {
				//IDLE
				m_playerState = IDLE;

				//WALK
				if (velocity.x > MIN_SPEED) {
					m_playerState = WALK;
				}
				else if (velocity.x < -MIN_SPEED) {
					m_playerState = WALK;
				}

			}
		}
	}

	//frame counter for time stuff
	static float frameCounter = 0.0f;

	//reset imageCounter and frameCounter when texture changed
	if (m_texture != m_textures[m_playerState])  {
		m_texture = m_textures[m_playerState];
		m_imageCounter = 0;
		frameCounter = 0.0f;
	}

	//set total frames
	float totalFrames = m_texture->ids.size()*7.0f;
	if (m_isPending) {
		totalFrames = 10.0f;
	}

	//update frame counter and texture image counter
	if (frameCounter < totalFrames/m_texture->ids.size())
	{
		frameCounter += deltaTime;
	}
	else
	{
		frameCounter = 0.0f;
		m_imageCounter++;
	}

	//loop if is not a pending activity
	//else stop pending
	if (m_imageCounter > m_texture->ids.size() - 1){
		if (m_isPending) {
			m_isPending = false;
			m_imageCounter--;
		}
		else {
			m_imageCounter = 0;
		}
	}

	//decide the uv rect
	if (velocity.x > MIN_SPEED) {
		m_UVdesRec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (velocity.x < -MIN_SPEED) {
		m_UVdesRec = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	}

}

void Player::startContact(b2Contact* contact) {
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	if (!m_isOnGround) {
		for (int i = 0;i < b2_maxManifoldPoints;i++) {
			if (manifold.points[i].y < m_capsule.getpos().y - m_capsule.getDim().y / 2.0f + m_capsule.getDim().x / 2.0f)
				m_isOnGround = true;
			break;
		}
	}
	m_contactNUM++;
}

void Player::endContact(b2Contact* contact) {
	m_contactNUM--;
	if (m_contactNUM == 0) {
		m_isOnGround = false;
	}
}

void Player::draw(Lengine::SpriteBatch* spritebatch) {
	spritebatch->draw(getRenderDesRect(), m_UVdesRec, m_texture->ids[m_imageCounter], 1.0f, m_color);
}

void Player::debugDraw(Lengine::DebugRender* debugrender) {
	debugrender->drawCapsule(getCollisionDesRect(), Lengine::ColorRGBA8(255, 255, 255, 255));
}

void Player::tempDraw(Lengine::SpriteBatch* spritebatch) {
	glm::vec4 desRec(m_tempPos, m_renderDim);
	spritebatch->draw(desRec, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture->ids[0], 1.0f, m_color);
}

void Player::tempDebugDraw(Lengine::DebugRender* debugRender, bool isSelected /*= false*/) {
	if (isSelected) {
		debugRender->drawCapsule(getCollisionDesRect(), Lengine::ColorRGBA8(255, 255, 0, 255));
	}
	else {
		debugRender->drawCapsule(getCollisionDesRect(), Lengine::ColorRGBA8(255, 255, 255, 255));
	}
}

bool Player::isInPlayer(const glm::vec2& pos) {
	glm::vec2 temp = pos - m_tempPos;
	if (abs(temp.x) < m_collisoinDim.x / 2.0f && abs(temp.y) < m_collisoinDim.y / 2.0f)
		return true;
	else
		return false;
}

glm::vec4 Player::getRenderDesRect() {

	glm::vec4 desRect = glm::vec4(m_tempPos, m_renderDim);

	if (m_capsule.getbody()) {
		desRect = glm::vec4(m_capsule.getpos() - m_posOffset,m_renderDim);
	}

	return desRect;
}

glm::vec4 Player::getCollisionDesRect() {

	glm::vec4 desRect(m_tempPos + m_posOffset, m_collisoinDim);

	//TODO: collision Dim should be change according to  shape change
	if (m_capsule.getbody()) {
		desRect = glm::vec4(m_capsule.getpos(), m_collisoinDim);
	}

	return desRect;
}