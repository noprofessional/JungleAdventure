#include "SpriteBatch.h"
#include"ResourceManager.h"
namespace Lengine {

	Glyph::Glyph() {}

	Glyph::Glyph(const glm::vec4 &recpos, const glm::vec4& recuv
		, const GLuint& TextureID, const float & Depth, const ColorRGBA8 &color) {
		depth = Depth;
		texture = TextureID;
		glm::vec2 temp;
		glm::vec2 pos;
		for (int x = 0;x < 2;x++) {
			for (int y = 0;y < 2;y++) {
				//set color
				vertices[x][y].color = color;
				//get corner position after rotation
				pos.x = recpos.x + (2 * x - 1)*recpos.z / 2.0f;
				pos.y = recpos.y + (2 * y - 1)*recpos.w / 2.0f;
				//set the position and uvcords
				vertices[x][y].setposition(pos.x, pos.y, depth);
				vertices[x][y].setUV(recuv.x + x*recuv.z, recuv.y + y*recuv.w);
			}
		}
	}

	Glyph::Glyph(const glm::vec4 &recpos, const glm::vec4& recuv
		, const GLuint& TextureID, const float & Depth
		, const ColorRGBA8 &color, const glm::vec2& direct)
	{
		depth = Depth;
		texture = TextureID;
		glm::vec2 temp;
		glm::vec2 pos;

		for (int x = 0;x < 2;x++) {
			for (int y = 0;y < 2;y++) {
				//set color
				vertices[x][y].color = color;
				//get a vector from center to the corners
				temp.x = (x * 2 - 1)*recpos.z / 2.0f;
				temp.y = (y * 2 - 1)*recpos.w / 2.0f;
				//get corner position after rotation
				pos.x = direct.x* temp.x - direct.y* temp.y + recpos.x;
				pos.y = direct.y* temp.x + direct.x* temp.y + recpos.y;
				//set the position and uvcords
				vertices[x][y].setposition(pos.x, pos.y, depth);
				vertices[x][y].setUV(recuv.x + x*recuv.z, recuv.y + y*recuv.w);
			}
		}
	}

	SpriteBatch::SpriteBatch() {}

	SpriteBatch::~SpriteBatch() {}
	//creat Vao and preset attribute
	void SpriteBatch::init() {
		createVertextArray();
	}

	//clear everything to prepare to render next frame
	void SpriteBatch::begin(GlyphSortType glyphtype/* defult : Texture*/) {
		//set sort type
		_sortType = glyphtype;
		//clear all directories
		_renderBatches.clear();
		//clear all glyphs
		_glyphs.clear();
		_glyphspointers.clear();
	}

	//add a basic no texture glypth into thd Glyphs
	void SpriteBatch::draw(const glm::vec4& recpos, const float & depth, const ColorRGBA8& color) {
		TextureCache temp;
		static GLtexture whitePNG= temp.gettexture("Textures/white.png");
		draw(recpos, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), whitePNG.ids[0], depth, color);
	}

	void SpriteBatch::draw(const glm::vec4 &recpos, const glm::vec4& recuv, const GLuint& textureID
		, const float & depth, const ColorRGBA8 &color/* = Lengine::ColorRGBA8(255, 255, 255, 255)*/, const glm::vec2 direct /*= glm::vec2(0.0f, 0.0f)*/) {
		glm::vec2 norDirect;
		//add the right frame to glyph
		if (direct.x == 0 && direct.y == 0)
		{
			_glyphs.emplace_back(recpos, recuv,textureID, depth, color);
		}
		else
		{
			norDirect = normalize(direct);
			_glyphs.emplace_back(recpos, recuv, textureID, depth, color, norDirect);
		}
	}
	void SpriteBatch::draw(const glm::vec4 &recpos, const glm::vec4& recuv, const GLuint& textureID
		, const float & depth, const ColorRGBA8 &color, const float& angle) {
		if (angle == 0.0f) {
			//if has no angle use default parameter
			draw(recpos, recuv, textureID, depth, color);
		}
		else {
			glm::vec2 norDirect(cos(angle), sin(angle));
			draw(recpos, recuv, textureID, depth, color,norDirect);
		}
	}


	//sort the Glyphs and creat vertices and renderbatches, sorter and processer
	void SpriteBatch::end() {
		//create glyphs pointer vector for easy sort
		_glyphspointers.resize(_glyphs.size());
		for (int i = 0;i < _glyphs.size();i++) {
			_glyphspointers[i] = &_glyphs[i];
		}
		//sort all glyphs by texture id
		sortGlyph();
		//create vertices vector
		//unzip rectangular to 6 vertices
		//create render directory
		//upload data
		createRenderBatches();
	}

	//use the vertices and renderbatches to render, user 
	void SpriteBatch::renderBatch() {
		glBindVertexArray(_vaoID);

		for (int i = 0;i < _renderBatches.size();i++) {
			glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);
			glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
		}
		glBindVertexArray(0);
	}

	//creat a vertex array object
	//enable atrribute to make it visible for render
	//preset attribute pointer to rearrange data uploaded
	void SpriteBatch::createVertextArray() {

		if (_vaoID == 0) {
			glGenVertexArrays(1, &_vaoID);
		}
		glBindVertexArray(_vaoID);


		if (_vboID == 0) {
			glGenBuffers(1, &_vboID);
		}
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

		/*If enabled, the values in the generic vertex attribute array will be accessed
		and used for rendering when calls are made to vertex array
		commands such as glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.*/
		glEnableVertexAttribArray(POS);
		glEnableVertexAttribArray(COL);
		glEnableVertexAttribArray(UVC);

		//this is vertex position attribute pointer(index,num,type,stride,startptr)
		glVertexAttribPointer(POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		//this is vertex color attribute pointer
		glVertexAttribPointer(COL, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		//this is vertex textureuv attribute pointer
		glVertexAttribPointer(UVC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glBindVertexArray(0);
	}
	void SpriteBatch::sortGlyph() {
		switch (_sortType) {
		case GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(_glyphspointers.begin(), _glyphspointers.end(), compareBacktoFront);
			break;
		case GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(_glyphspointers.begin(), _glyphspointers.end(), compareFronttoBack);
			break;
		case GlyphSortType::TEXTURE:
			std::stable_sort(_glyphspointers.begin(), _glyphspointers.end(), compareTexture);
			break;
		}

	}
	//create vertices vector
	//unzip rectangular to 6 vertices
	//create render directory
	//upload data
	void SpriteBatch::createRenderBatches() {
		std::vector<Vertex> vertices;
		vertices.resize(_glyphs.size() * 6);

		if (_glyphs.empty()) {
			return;
		}
		int offset = 0;
		int cv = 0; //current vertice
					//emplace_back build a object in an already allocated storage.
		_renderBatches.emplace_back(offset, 6, _glyphspointers[0]->texture);
		vertices[cv++] = _glyphspointers[0]->topleft;
		vertices[cv++] = _glyphspointers[0]->bottomleft;
		vertices[cv++] = _glyphspointers[0]->topright;
		vertices[cv++] = _glyphspointers[0]->bottomleft;
		vertices[cv++] = _glyphspointers[0]->topright;
		vertices[cv++] = _glyphspointers[0]->bottomright;
		offset += 6;

		for (int cg = 1;cg < _glyphs.size();cg++) {
			if (_glyphspointers[cg]->texture != _glyphspointers[cg - 1]->texture)
				//if it has different texture  create a new batch
				_renderBatches.emplace_back(offset, 6, _glyphspointers[cg]->texture);
			else
				//if it has same texture increase numVertices in current batch by 6
				_renderBatches.back().numVertices += 6;
			vertices[cv++] = _glyphspointers[cg]->topleft;
			vertices[cv++] = _glyphspointers[cg]->bottomleft;
			vertices[cv++] = _glyphspointers[cg]->topright;
			vertices[cv++] = _glyphspointers[cg]->bottomleft;
			vertices[cv++] = _glyphspointers[cg]->topright;
			vertices[cv++] = _glyphspointers[cg]->bottomright;
			//keep track of how many vertices has been created data for
			offset += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

		//ophan the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		//upload the data
		glBufferSubData(GL_ARRAY_BUFFER,0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	bool SpriteBatch::compareFronttoBack(Glyph*a, Glyph*b) {
		return (a->depth < b->depth);
	}
	bool SpriteBatch::compareBacktoFront(Glyph*a, Glyph*b) {
		return (a->depth > b->depth);
	}
	bool SpriteBatch::compareTexture(Glyph*a, Glyph*b) {
		return (a->texture < b->texture);
	}

}