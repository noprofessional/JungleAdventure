#include "Light.h"
#include <fstream>
float SELECT_RADIUS = 0.5f;
const Lengine::ColorRGBA8 MAGENTA(255, 255, 0, 255);
const Lengine::ColorRGBA8 WHITE(255, 255, 255, 255);

void Light::tempSetAll(const glm::vec2& CenterPos, float Size, const Lengine::ColorRGBA8& Color) {
	centerPos = CenterPos;
	size = Size;
	color = Color;
}

void Light::draw(Lengine::SpriteBatch* spriteBatch) {
	glm::vec4 desRec(centerPos.x, centerPos.y, 2 *size, 2 *size);
	spriteBatch->draw(desRec, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, 1.0f,color);
}

void Light::debugDraw(Lengine::DebugRender* debugRenderer, bool selected /*= false*/) {
	if (!selected) {
		debugRenderer->drawCircle(centerPos, SELECT_RADIUS, WHITE);
		debugRenderer->drawCircle(centerPos, size, WHITE);
	}
	else {
		debugRenderer->drawCircle(centerPos, SELECT_RADIUS, MAGENTA);
		debugRenderer->drawCircle(centerPos, size, WHITE);
	}
}

bool Light::isInLight(const glm::vec2&currentPos) {
	glm::vec2 desVec(currentPos - centerPos);
	if (glm::length(desVec) < SELECT_RADIUS)
		return true;
	else
		return false;
}

void Light::writeAsBinary(std::ofstream& fout) const{
	fout.write((char*)&centerPos, sizeof(centerPos));
	fout.write((char*)&size, sizeof(size));
	fout.write((char*)&color, sizeof(color));
}
void Light::readFromBinary(std::ifstream& fin) {
	fin.read((char*)&centerPos, sizeof(centerPos));
	fin.read((char*)&size, sizeof(size));
	fin.read((char*)&color, sizeof(color));
}