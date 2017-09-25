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
	void debugDraw(Lengine::DebugRender* dr,bool selected = false);
	bool inTile(const glm::vec2& mouseCords);

	void writeAsText(std::ofstream& fout) const;
	void readFromText(std::ifstream& fin);

	glm::vec4 getDesRec() { return m_desRec; }
	void offsetPos(const float xOffset, const float& yOffset) { m_desRec.x += xOffset; m_desRec.y += yOffset; }
	void showInfo(glm::vec4& desRec, glm::vec4& uvRec, std::string& texturePath, float& depth) const;
	bool operator<(const Tile& B)const { return this->m_depth < B.m_depth; }

private:
	glm::vec4 m_desRec;
	glm::vec4 m_uvRec;
	Lengine::GLtexture* m_texture;
	float m_depth;
};

class Tiles {
public:
	void addTile(Tile& tile);
	bool selectTile(const glm::vec2& mouseCords, bool more);
	void nextCandidate();
	void deleteTile(const glm::vec2& mouseCords);

	void draw(Lengine::SpriteBatch* sb);
	void drawTexture(Lengine::SpriteBatch* sb,Lengine::Camera* cam);
	void debugDraw(Lengine::DebugRender* dr, bool debugging);

	void writeAsText(std::ofstream& fout)const;
	void readFromText(std::ifstream& fin);

	void renderToTexture(Lengine::Camera cam);
	void writeAsBinary(std::ofstream& fout) const;
	void readFromBinary(std::ifstream& fin);

	void showSelectedInfo(glm::vec4& desRec, glm::vec4& uvRec, std::string& texturePath, float& depth)const;
	void offsetSelectedPos(const float& xOffset, const float& yOffset);
	void replaceSelected(Tile& tile);
	void clearSelection() { m_candidateIndex.clear();m_selectedIndex.clear(); }
	bool hasSelection()const;

private:
	glm::vec4 getArea();

	std::vector<Tile> m_tiles;
	std::vector<int> m_candidateIndex;
	std::vector<int> m_selectedIndex;

	int m_texWidth;
	int m_texHeight;
	std::vector<unsigned char> m_textureData;
	Lengine::GLtexture* m_texture;
};