#pragma once
#include<Lengine\Vertex.h>
#include<glm\glm.hpp>
#include<Lengine\SpriteBatch.h>
#include<Lengine/DebugRender.h>
class Light
{
public:
	void draw(Lengine::SpriteBatch* spriteBatch);
	void debugDraw(Lengine::DebugRender* debugRenderer,bool selected= false);

	glm::vec2 centerPos;
	float size;
	Lengine::ColorRGBA8 color;
};

