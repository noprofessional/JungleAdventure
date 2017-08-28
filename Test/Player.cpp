#include"Player.h"
#include<algorithm>

Player::Player(){

}

Player::~Player() {

}

void Player::tempSetAll(const glm::vec4 &RenderDesRec,const Lengine::GLtexture& Texture) {
	m_texture = Texture;
	m_tempPos = glm::vec2(RenderDesRec.x, RenderDesRec.y);
	m_renderDim = glm::vec2(RenderDesRec.z, RenderDesRec.w);

	m_color = Lengine::ColorRGBA8(255, 255, 255, 255);
}
void Player::addToWorld(b2World* world, const glm::vec2& CollisionDim,glm::vec2 PosOffset) {
	glm::vec4 CollisionDesRec(m_tempPos+PosOffset, CollisionDim);
	m_capsule.init(world,CollisionDesRec,0.0f, true);
	m_renderPosOffset = -PosOffset;
}

void Player::update(Lengine::InputManager* inputmanager) {
	const float MAX_SPEED = 5.0f;
	b2Body* temp = m_capsule.getbody();
	if (inputmanager->isKEYdown(SDLK_a)) {
		temp->ApplyForceToCenter(b2Vec2(-10.0f, 0.0f), true);
	}else if (inputmanager->isKEYdown(SDLK_d)) {
		temp->ApplyForceToCenter(b2Vec2(10.0f, 0.0f), true);
	}else {
		temp->SetLinearVelocity(b2Vec2(0.95*temp->GetLinearVelocity().x, temp->GetLinearVelocity().y));
	}

	float Xspeed = temp->GetLinearVelocity().x;
	if (Xspeed > MAX_SPEED) {
		Xspeed = MAX_SPEED;
	}
	else if (Xspeed < -MAX_SPEED) {
		Xspeed = -MAX_SPEED;
	}
	temp->SetLinearVelocity(b2Vec2(Xspeed,temp->GetLinearVelocity().y));

	for (b2ContactEdge *ce = temp->GetContactList();ce!= nullptr;ce = ce->next){
		b2Contact* c = ce->contact;
		if (c->IsTouching()) {
			b2WorldManifold manifold;
			c->GetWorldManifold(&manifold);
			bool below = false;
			for(int i = 0;i < b2_maxManifoldPoints;i++) {
				if (manifold.points[i].y < m_capsule.getpos().y - m_capsule.getDim().y / 2.0f +0.1f)
					below = true;
					break;
			}
			if (below) {
				if (inputmanager->isKEYpressed(SDLK_w)){
					temp->ApplyLinearImpulse(b2Vec2(0.0f, 5.0f), b2Vec2(temp->GetPosition()), true);
					break;
				}
			}
		}
	}

}

void Player::draw(Lengine::SpriteBatch* spritebatch,float deltaTime/*=1.0f*/) {
	static float frameCounter = 0.0f;
	static int imageCounter = 0;

	if (frameCounter < 16.0f) 
	{
		frameCounter + deltaTime;
	}
	else 
	{
	frameCounter = 0.0f;
	if (++imageCounter >= m_texture.ids.size())
	{
		imageCounter = 0;
	}
	}
	glm::vec2 RenderPos = m_capsule.getpos()+m_renderPosOffset;
	glm::vec4 desRec(RenderPos.x,RenderPos.y, m_renderDim.x, m_renderDim.y);
	spritebatch->draw(desRec, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.ids[imageCounter], 1.0f, m_color);
}
void Player::tempDraw(Lengine::SpriteBatch* spritebatch) {
	glm::vec4 desRec(m_tempPos, m_renderDim);
	spritebatch->draw(desRec, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.ids[0], 1.0f, m_color);
}

void Player::debugDraw(Lengine::DebugRender* debugrender) {
	m_capsule.debugdraw(debugrender);
}
void Player::tempDebugDraw(Lengine::DebugRender* debugRender,bool isSelected /*= false*/) {
	glm::vec2 collisionPosOffset = -m_renderPosOffset;
	glm::vec2 CollisionPos = m_tempPos + collisionPosOffset;
	glm::vec4 desRec(CollisionPos.x, CollisionPos.y, m_capsule.getDim().x, m_capsule.getDim().y);
	if (isSelected) {
		debugRender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 0, 255), 0);
	}
	else {
		debugRender->drawCapsule(desRec, Lengine::ColorRGBA8(255, 255, 255, 255), 0);
	}
}
