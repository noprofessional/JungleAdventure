#include "Light.h"
#include <fstream>
float SELECT_RADIUS = 0.5f;
const Lengine::ColorRGBA8 MAGENTA(255, 0, 255, 255);
const Lengine::ColorRGBA8 WHITE(255, 255, 255, 255);

//light
Light::Light() {

}

Light::Light(const glm::vec2& centerPos, float radius, const Lengine::ColorRGBA8& color) {
	m_centerPos = centerPos;
	m_radius = radius;
	m_color = color;
}

Light::~Light() {

}

void Light::draw(Lengine::SpriteBatch* spriteBatch) {
	glm::vec4 desRec(m_centerPos.x, m_centerPos.y, 2 *m_radius, 2 * m_radius);
	spriteBatch->draw(desRec, glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f), 0, 1.0f,m_color);
}

void Light::debugDraw(Lengine::DebugRender* debugRenderer, bool selected /*= false*/) {
	if (!selected) {
		debugRenderer->drawCircle(m_centerPos, SELECT_RADIUS, WHITE);
	}
	else {
		debugRenderer->drawCircle(m_centerPos, SELECT_RADIUS, MAGENTA);
	}
}

bool Light::isInLight(const glm::vec2&currentPos) {
	glm::vec2 desVec(currentPos - m_centerPos);
	if (glm::length(desVec) < SELECT_RADIUS)
		return true;
	else
		return false;
}

void Light::writeAsText(std::ofstream& fout)const {
	fout << m_centerPos.x << ' ' << m_centerPos.y << ' '
		<< m_radius << ' '
		<< m_color.r << ' ' << m_color.g << ' ' << m_color.b << ' ' << m_color.a << '\n';
}

void Light::readFromText(std::ifstream& fin) {
	fin >> m_centerPos.x >> m_centerPos.y
		>> m_radius
		>> m_color.r >> m_color.g >> m_color.b >> m_color.a;
}

void Light::writeAsBinary(std::ofstream& fout) const{
	fout.write((char*)&m_centerPos, sizeof(glm::vec2));
	fout.write((char*)&m_radius, sizeof(float));
	fout.write((char*)&m_color, sizeof(Lengine::ColorRGBA8));
}

void Light::readFromBinary(std::ifstream& fin) {
	fin.read((char*)&m_centerPos, sizeof(glm::vec2));
	fin.read((char*)&m_radius, sizeof(float));
	fin.read((char*)&m_color, sizeof(Lengine::ColorRGBA8));
}

void Light::showInfo(glm::vec2& centerPos, float& radius, Lengine::ColorRGBA8 & color) {
	centerPos = m_centerPos;
	radius = m_radius;
	color = m_color;
}


//lights
Lights::Lights() {
	m_selectedIndex = -1;
}

Lights::~Lights() {

}

void Lights::addLight(const Light& light) {
	m_lights.push_back(light);
}

bool Lights::selectLight(const glm::vec2& mouseCords) {
	m_selectedIndex = -1;
	for (int i = 0;i < m_lights.size();i++) {
		if (m_lights[i].isInLight(mouseCords)) {
			m_selectedIndex = i;
			break;
		}
	}
	return hasSelection();
}

void Lights::deleteLight(const glm::vec2& mouseCords) {
	selectLight(mouseCords);
	if (hasSelection()) {
		m_lights[m_selectedIndex] = m_lights.back();
		m_lights.pop_back();
		m_selectedIndex = -1;
	}
}

void Lights::draw(Lengine::SpriteBatch* spriteBatch) {
	for (auto& L : m_lights) {
		L.draw(spriteBatch);
	}
}

void Lights::debugDraw(Lengine::DebugRender* debugRender,bool debugging) {
	if (debugging) {
		for (auto& L : m_lights) {
			L.debugDraw(debugRender);
		}
	}
	if (hasSelection()) {
		m_lights[m_selectedIndex].debugDraw(debugRender, true);
	}
}

void Lights::writeAsText(std::ofstream& fout)const {
	fout << m_lights.size() << '\n';
	for (auto& L : m_lights) {
		L.writeAsText(fout);
	}
}

void Lights::readFromText(std::ifstream& fin) {
	int size;
	fin >> size;
	m_lights.resize(size);
	for (auto& L : m_lights) {
		L.readFromText(fin);
	}
}

void Lights::writeAsBinary(std::ofstream& fout)const {
	int size = m_lights.size();
	fout.write((char*)&size, sizeof(int));
	for (auto& L : m_lights) {
		L.writeAsBinary(fout);
	}
}

void Lights::readFromBinary(std::ifstream& fin) {
	int size;
	fin.read((char*)& size, sizeof(int));
	m_lights.resize(size);
	for (auto& L : m_lights) {
		L.readFromBinary(fin);
	}
}

void Lights::showSelectedInfo(glm::vec2& centerPos, float& radius, Lengine::ColorRGBA8 & color) {
	if (hasSelection()) {
		m_lights[m_selectedIndex].showInfo(centerPos, radius, color);
	}
}

void Lights::offsetSelectedPos(const float& xOffset, const float& yOffset) {
	if (hasSelection()) {
		glm::vec2 pos;
		float rasius;
		Lengine::ColorRGBA8 color;
		m_lights[m_selectedIndex].showInfo(pos, rasius, color);

		pos.x += xOffset;
		pos.y += yOffset;
		m_lights[m_selectedIndex] = Light(pos, rasius, color);
	}
}

void Lights::replaceSelected(const Light& light) {
	if (hasSelection()) {
		m_lights[m_selectedIndex] = light;
	}
}

