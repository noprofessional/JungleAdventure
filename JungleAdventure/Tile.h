#pragma once
#include <glm/glm.hpp>
#include <Lengine/TextureCache.h>
class Tile
{
public:
	Tile();
	~Tile();
	glm::vec2 Pos;
	glm::vec2 Dim;
	glm::vec4 uvCords;
	Lengine::GLtexture texture;
};

