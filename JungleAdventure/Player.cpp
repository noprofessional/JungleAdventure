#include"Player.h"
#include <fstream>

const float MAX_WALK_SPEED = 4.0f;
const float MAX_RUN_SPEED = 7.0f;
const float MIN_SPEED = 0.1f;

Player::Player() {

}

Player::Player(const glm::vec4 &RenderDesRec, const glm::vec2& CollisionDim, const glm::vec2& PosOffset) {
	m_renderPos = glm::vec2(RenderDesRec.x, RenderDesRec.y);
	m_renderDim = glm::vec2(RenderDesRec.z, RenderDesRec.w);
	m_collisionDim = CollisionDim;
	m_posOffset = PosOffset;
	init();
}

Player::~Player() {

}

void Player::addToWorld(b2World* world) {

	m_capsule.addToWorld(world);
	m_capsule.getbody()->SetUserData(this);

	m_limitSpeed = MAX_WALK_SPEED;

	//get all texture pointer
	m_textures[IDLE] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/IDLE.gif");
	m_textures[JUMP_UP] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/JUMP_1_UP.gif");
	m_textures[JUMP_MIDAIR] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/JUMP_2_MIDAIR.gif");
	m_textures[JUMP_DOWN] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/JUMP_3_DOWN.gif");
	m_textures[JUMP_LAND] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/JUMP_4_LAND.gif");
	m_textures[WALK] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/WALK.gif");
	m_textures[RUN] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/RUN.gif");
	m_textures[DUCK] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/DUCK.gif");
	m_textures[SLIDE] =  Lengine::textureCache.getSTClampedTexture("Textures/Player/SLIDE.gif");

	m_move = glm::vec2(0.0f);
	m_preCPos = m_capsule.getpos();
}

void Player::update(Lengine::InputManager* inputmanager, float deltaTime/*=1.0f*/) {

	m_move = m_capsule.getpos() - m_preCPos;
	m_preCPos = m_capsule.getpos();

	b2Body* temp = m_capsule.getbody();

	if (!temp) return;
	b2Vec2 velocity = temp->GetLinearVelocity();

	//double click for run
	if (m_limitSpeed != MAX_RUN_SPEED)
	{
		if (inputmanager->isKEYdoubleClicked(SDLK_d) || inputmanager->isKEYdoubleClicked(SDLK_a)) {
			m_limitSpeed = MAX_RUN_SPEED;
		}
	}

	//movement on X-axis
	float force = 3.0f;	
	float dampingForce = 5.0f;
	if (m_isOnGround) {
		force = 12.0f;
		dampingForce = 10.0f;
	}
	if (inputmanager->isKEYdown(SDLK_a)) {
		temp->ApplyForceToCenter(b2Vec2(-force, 0.0f), true);
	}
	else if (inputmanager->isKEYdown(SDLK_d)) {
		temp->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
	}
	else if (velocity.x >MIN_SPEED) {
		temp->ApplyForceToCenter(b2Vec2(-dampingForce, 0.0f), true);
	}
	else if (velocity.x <-MIN_SPEED) {
		temp->ApplyForceToCenter(b2Vec2(dampingForce, 0.0f), true);
	}
	else {
		temp->SetLinearVelocity(b2Vec2(0.0f, velocity.y));
		m_limitSpeed = MAX_WALK_SPEED;
	}
	
	//limit the X-axised move speed
	if (velocity.x > m_limitSpeed) {
		temp->SetLinearVelocity(b2Vec2(m_limitSpeed, velocity.y));
	}
	else if (velocity.x < -m_limitSpeed) {
		temp->SetLinearVelocity(b2Vec2(-m_limitSpeed, velocity.y));
	}

	//movement on Y-axis
	if (m_isOnGround) {
		if (inputmanager->isKEYpressed(SDLK_w)) {
			m_playerState = DUCK;
			m_isPending = true;
		}
	}
	else {
		inputmanager->releasekey(SDLK_w);
	}

	//state decide
	if (!m_isPending) {
		if (m_playerState == DUCK) {
			temp->ApplyLinearImpulse(b2Vec2(0.0f, 14.0f), b2Vec2(temp->GetPosition()), true);
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
				if (abs(velocity.x) > MIN_SPEED) {
					m_playerState = WALK;
				}

				//RUN
				if (abs(velocity.x) > MAX_WALK_SPEED+MIN_SPEED) {
					m_playerState = RUN;
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
		totalFrames = 5.0f;
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
	//else use last image
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
		m_UVdesRec = glm::vec4(1.0f, 0.0f, -1.0f, 1.0f);
	}

}

void Player::startContact(b2Contact* contact) {
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);
	if (!m_isOnGround) {
		for (int i = 0;i < b2_maxManifoldPoints;i++) {
			if (manifold.points[i].y < m_capsule.getpos().y - m_collisionDim.y / 2.0f + m_collisionDim.x / 4.0f)
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
	if (!m_isInit) return;
	spritebatch->draw(getRenderDesRect(), m_UVdesRec, m_texture->ids[m_imageCounter], 20.0f, m_color);
}

void Player::debugDraw(Lengine::DebugRender* debugrender) {
	if (!m_isInit) return;
	m_capsule.debugdraw(debugrender, m_isSelected);
}

bool Player::selectPlayer(const glm::vec2& mouseCords) {
	m_isSelected = false;
	if (m_capsule.inCapsule(mouseCords))
		m_isSelected = true;
	return m_isSelected;
}

void Player::writeAsText(std::ofstream& fout)const {
	if (!m_isInit) {
		printf("The player has not been initialized!\nNothing is saved!\n");
		fout << 0 << '\n';
		return;
	}
	fout << 1 << '\n';
	fout << m_renderPos.x << ' ' << m_renderPos.y << ' '
		<< m_renderDim.x << ' ' << m_renderDim.y << ' '
		<< m_collisionDim.x << ' ' << m_collisionDim.y << ' '
		<< m_posOffset.x << ' ' << m_posOffset.y << '\n';
}

void Player::readFromText(std::ifstream& fin) {
	int a;
	fin >> a;
	if (a == 0) {
		printf("No player in the text file!\nLoad nothing!\n");
		return;
	}
	fin >> m_renderPos.x >> m_renderPos.y
		>> m_renderDim.x >> m_renderDim.y
		>> m_collisionDim.x >> m_collisionDim.y
		>> m_posOffset.x >> m_posOffset.y;

	init();
}

void Player::writeAsBinary(std::ofstream& fout)const {
	fout.write((char*)&m_renderPos, sizeof(glm::vec2));
	fout.write((char*)&m_renderDim, sizeof(glm::vec2));
	fout.write((char*)&m_collisionDim, sizeof(glm::vec2));
	fout.write((char*)&m_posOffset, sizeof(glm::vec2));
}

void Player::readFromBinary(std::ifstream& fin) {

	fin.read((char*)&m_renderPos, sizeof(glm::vec2));
	fin.read((char*)&m_renderDim, sizeof(glm::vec2));
	fin.read((char*)&m_collisionDim, sizeof(glm::vec2));
	fin.read((char*)&m_posOffset, sizeof(glm::vec2));

	init();
}

void Player::changePos(const float& xOffset, const float & yOffset) {
	m_renderPos += glm::vec2(xOffset, yOffset);
	m_capsule = Capsule(glm::vec4(m_renderPos + m_posOffset, m_collisionDim), 0.0f, true);
}

glm::vec2 Player::getMove() {
	return m_move;
}

glm::vec4 Player::getRenderDesRect() {

	glm::vec4 desRect = glm::vec4(m_renderPos, m_renderDim);

	if (m_capsule.getbody()) {
		desRect = glm::vec4(m_capsule.getpos() - m_posOffset,m_renderDim);
	}

	return desRect;
}

void Player::init() {
	m_UVdesRec = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	m_texture = Lengine::textureCache.getSTClampedTexture("Textures/Player/IDLE.gif");
	m_color = Lengine::ColorRGBA8(255, 255, 255, 255);
	m_imageCounter = 0;
	m_capsule = Capsule(glm::vec4(m_renderPos + m_posOffset, m_collisionDim), 0.0f, true);
	m_contactNUM = 0;
	m_isInit = true;
}
