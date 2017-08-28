#include "Light.h"
const float SELECT_RADIUS = 0.5f;
const Lengine::ColorRGBA8 MAGENTA(255, 255, 0, 255);
const Lengine::ColorRGBA8 WHITE(255, 255, 255, 255);
void Light::draw(Lengine::SpriteBatch* spriteBatch) {
	glm::vec4 desRec(centerPos.x, centerPos.y, 2 *size, 2 *size);
	spriteBatch->draw(desRec, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, 1.0f,color);
}
void Light::debugDraw(Lengine::DebugRender* debugRenderer, bool selected /*= false*/) {
	if (!selected)
		debugRenderer->drawCircle(centerPos, SELECT_RADIUS, WHITE);
	else {
		debugRenderer->drawCircle(centerPos, SELECT_RADIUS, MAGENTA);
		debugRenderer->drawCircle(centerPos, size, WHITE);
	}
}
