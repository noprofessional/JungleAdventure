#pragma once
#include<Lengine\Vertex.h>
#include<glm\glm.hpp>
#include<Lengine\SpriteBatch.h>
#include<Lengine/DebugRender.h>
class Light
{
public:
	void tempSetAll(const glm::vec2& centerPos, float size, const Lengine::ColorRGBA8& color);
	void draw(Lengine::SpriteBatch* spriteBatch);
	void debugDraw(Lengine::DebugRender* debugRenderer,bool selected= false);
	bool isInLight(const glm::vec2&currentPos);

	glm::vec2 centerPos;
	float size;
	Lengine::ColorRGBA8 color;
};

