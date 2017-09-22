#pragma once
#include <vector>
#include <glm/glm.hpp>

#include <Lengine/Camera.h>
#include <Lengine/TextureCache.h>
#include <Lengine/SpriteBatch.h>
#include <Lengine/DebugRender.h>

class Tile
{
public:
	Tile();
	Tile(const glm::vec4& DesRec, const glm::vec4& UvRec, Lengine::GLtexture* Texture,float Depth);
	~Tile();
	void draw(Lengine::SpriteBatch* spriteBatch);
	void debugDraw(Lengine::DebugRender* dr,bool selected);
	bool inTile(const glm::vec2& mouseCords);

	void writeAsText(std::ofstream& fout);
	void readFromText(std::ifstream& fin);

	float getPositionX() { return m_desRec.x; }
	float getPositionY() { return m_desRec.y; }

private:
	glm::vec4 m_desRec;
	glm::vec4 m_uvRec;
	Lengine::GLtexture* m_texture;
	float m_depth;
};

class Tiles {
public:
	void addTile(Tile& tile);
	void selectTile(const glm::vec2& mouseCords, bool more);
	void deleteTile();

	void draw(Lengine::SpriteBatch* sb);
	void drawAsTexture(Lengine::SpriteBatch*sb);
	void debugDraw(Lengine::DebugRender* dr, bool debugging);

	void writeAsText(std::ofstream& fout);
	void reasAsText(std::ifstream& fin);

	void renderToTexture(Lengine::Camera cam);
	void writeAsBinary(std::ofstream& fout);
	void readAsBinary(std::ifstream& fin);

private:
	glm::vec2 getArea();

	std::vector<Tile> m_tiles;
	std::vector<int> m_candidateIndex;
	std::vector<int> m_selectedIndex;

	int m_texWidth;
	int m_texHeight;
	std::vector<unsigned char> m_textureData;
};

class RenderToTexture {
public:
	static void RenderTiles(Tiles& tiles, Lengine::Camera cam);
};