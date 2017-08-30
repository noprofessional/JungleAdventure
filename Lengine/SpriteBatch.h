#pragma once
#include <GL\glew.h>
#include "Vertex.h"
#include <glm\glm.hpp>
#include <vector>
#include <algorithm>
#include "TextureCache.h"
namespace Lengine {

enum class GlyphSortType {
	NONE,
	FRONT_TO_BACK,
	BACK_TO_FRONT,
	TEXTURE
};

class Glyph {
public:
	Glyph();

	Glyph(const glm::vec4 &recpos, const glm::vec4& recuv
		, const GLuint& TextureID, const float & Depth, const ColorRGBA8 &color);

	Glyph(const glm::vec4 &recpos, const glm::vec4& recuv
		, const GLuint& TextureID, const float & Depth
		, const ColorRGBA8 &color, const glm::vec2& direct);
	GLuint texture;
	float depth;

	Vertex& bottomleft = vertices[0][0];
	Vertex& bottomright = vertices[1][0];
	Vertex& topleft = vertices[0][1];
	Vertex& topright = vertices[1][1];
private:
	//first index is X, second is Y
	//such as 0, 0 => left, bottom
	Vertex vertices[2][2];
};

class RenderBatch {
public:
	RenderBatch(GLuint Offset, GLuint NumVertices, GLuint Texture) :offset(Offset), numVertices(NumVertices), texture(Texture) {}
	GLuint offset;
	GLuint numVertices;
	GLuint texture;
};

class SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();
	
	//creat Vao and preset attribute
	void init();
	
	//clean all sprite batches and Glyphs, cleaner
	void begin(GlyphSortType glphtype = GlyphSortType::TEXTURE);
	
	//sort the Glyphs and creat vertices and renderbatches, sorter and processer
	void end();

	//add a basic no texture glypth into thd Glyphs
	void draw(const glm::vec4& recpos, const float & depth, const ColorRGBA8& color);
	
	//draw rotate boxes with direction vector
	void draw(const glm::vec4 &recpos, const glm::vec4& recuv,
		const GLuint& texture, const float & depth,
		const ColorRGBA8 &color=Lengine::ColorRGBA8(255,255,255,255),
		const glm::vec2 direction=glm::vec2(0.0f,0.0f));
	
	//draw rotate boxes with angle in degrees
	void draw(const glm::vec4 &recpos, const glm::vec4& recuv, const GLuint& texture
		, const float & depth, const ColorRGBA8 &color,const float& angle);

	//use the vertices and renderbatches to render, user 
	void renderBatch();
private:

	static bool compareFronttoBack(Glyph*a, Glyph*b);
	static bool compareBacktoFront(Glyph*a, Glyph*b);
	static bool compareTexture(Glyph*a, Glyph*b);

	void createRenderBatches();
	void createVertextArray();
	void sortGlyph();

	GlyphSortType _sortType;
	GLuint _vboID;
	GLuint _vaoID;

	std::vector<Glyph> _glyphs;
	std::vector<Glyph*> _glyphspointers; // work as a catalog for glyphs
	//work as a catalog for vertices created in createRenderBatches()
	std::vector<RenderBatch> _renderBatches; 
};
}