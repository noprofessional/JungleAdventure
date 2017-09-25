#pragma once
#include<Lengine\Vertex.h>
#include<glm\glm.hpp>
#include<Lengine\SpriteBatch.h>
#include<Lengine/DebugRender.h>
class Light
{
public:
	Light();
	Light(const glm::vec2& centerPos, float radius, const Lengine::ColorRGBA8& color);
	~Light();

	void draw(Lengine::SpriteBatch* spriteBatch);
	void debugDraw(Lengine::DebugRender* debugRenderer,bool selected= false);
	bool isInLight(const glm::vec2&currentPos);

	void writeAsText(std::ofstream& fout)const;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const;
	void readFromBinary(std::ifstream& fin);

	void showInfo(glm::vec2& centerPos, float& radius, Lengine::ColorRGBA8 & color);
private:
	glm::vec2 m_centerPos;
	float m_radius;
	Lengine::ColorRGBA8 m_color;
};

class Lights {
public:
	Lights();
	~Lights();
	void addLight(const Light& light);
	bool selectLight(const glm::vec2& mouseCords);
	void deleteLight(const glm::vec2& mouseCords);

	void draw(Lengine::SpriteBatch* spriteBatch);
	void debugDraw(Lengine::DebugRender* debugRender, bool debugging);

	void writeAsText(std::ofstream& fout)const;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const;
	void readFromBinary(std::ifstream& fin);

	void clearSelection() { m_selectedIndex = -1; }
	void showSelectedInfo(glm::vec2& centerPos, float& radius, Lengine::ColorRGBA8 & color);
	void offsetSelectedPos(const float& xOffset, const float& yOffset);
	void replaceSelected(const Light& light);
private:
	bool hasSelection() { return m_selectedIndex != -1; }

	std::vector<Light> m_lights;
	int m_selectedIndex;
};