#include "Tile.h"
#include <fstream>
#include <Lengine/FatalError.h>
#include <Lengine/ShaderPro.h>

//tile
Tile::Tile()
{
}

Tile::Tile(const glm::vec4& DesRec, const glm::vec4& UvRec, Lengine::GLtexture* Texture,float Depth) {
	if (DesRec.z < 0 || DesRec.w < 0) {
		Lengine::fatalError("Negative Tile Width(Height)!");
	}
	m_desRec = DesRec;
	m_uvRec = UvRec;
	m_texture = Texture;
	m_depth = Depth;
}

Tile::~Tile()
{
}

void Tile::draw(Lengine::SpriteBatch* SpriteBatch) {
	SpriteBatch->draw(m_desRec, m_uvRec, m_texture->ids[0], m_depth);
}

void Tile::debugDraw(Lengine::DebugRender* dr, bool selected) {
	if (selected) {
		dr->drawBox(m_desRec, Lengine::ColorRGBA8(255, 255, 0, 255), 0.0f);
	}
	else {
		dr->drawBox(m_desRec, Lengine::ColorRGBA8(255, 255, 255, 255), 0.0f);
	}
}

bool Tile::inTile(const glm::vec2& mouseCords) {
	glm::vec2 desVec;
	desVec.x = mouseCords.x - m_desRec.x;
	desVec.y = mouseCords.y - m_desRec.y;
	if (abs(desVec.x) < m_desRec.z && abs(desVec.y) < m_desRec.w)
		return true;
	else
		return false;
}

void Tile::writeAsText(std::ofstream& fout) {
	fout << m_desRec.x << ' ' << m_desRec.y << ' ' << m_desRec.z << ' ' << m_desRec.w << ' '
		<< m_uvRec.x << ' ' << m_uvRec.y << ' ' << m_uvRec.z << ' ' << m_uvRec.w << ' '
		<< m_texture->filePath.c_str() << ' '
		<< m_depth << '\n';
}

void Tile::readFromText(std::ifstream& fin) {
	fin >> m_desRec.x >> m_desRec.y >> m_desRec.z >> m_desRec.w
		>> m_uvRec.x >> m_uvRec.y >> m_uvRec.z >> m_uvRec.w
		>> m_texture->filePath
		>> m_depth;
}


//tiles
void Tiles::addTile(Tile& tile) {
	m_tiles.push_back(tile);
}

void Tiles::selectTile(const glm::vec2& mouseCords, bool more) {
	
	m_candidateIndex.clear();

	for (int i = 0;i < m_tiles.size();i++) {
		if (m_tiles[i].inTile(mouseCords)) {
			m_candidateIndex.push_back(i);
		}
	}
	
	if (m_candidateIndex.size() > 0) {
		if (more) {
			bool same = false;
			for (int& i : m_candidateIndex)
			{
				for (int& j : m_selectedIndex) {
					if (i == j) {
						j = m_selectedIndex.back();
						m_selectedIndex.pop_back();
						same = true;
					}
				}
				if (!same) {
					m_selectedIndex.push_back(i);
				}
			}
		}
		else {
			m_selectedIndex.resize(1);
			m_selectedIndex[0] = m_candidateIndex[0];
		}
	}

}

void Tiles::deleteTile() {

}

void Tiles::draw(Lengine::SpriteBatch* sb) {
	for (auto& T : m_tiles) {
		T.draw(sb);
	}
}

void Tiles::debugDraw(Lengine::DebugRender* dr,bool debugging) {
	if (debugging) {
		for (auto& T : m_tiles) {
			T.debugDraw(dr, false);
		}
	}

	for (auto&I : m_selectedIndex) {
		m_tiles[I].debugDraw(dr, true);
	}
}

void Tiles::writeAsText(std::ofstream& fout) {
	fout << m_tiles.size() << '\n';
	for (auto& T : m_tiles) {
		T.writeAsText(fout);
	}
}

void Tiles::reasAsText(std::ifstream& fin) {
	int size;
	fin >> size;
	m_tiles.resize(size);
	for (auto& T : m_tiles) {
		T.readFromText(fin);
	}
}

void Tiles::renderToTexture(Lengine::Camera cam) {

	//set program
	Lengine::ShaderPro program;
	program.compileshaders(std::string("Shaders/shaderpro.vert"), std::string("Shaders/shaderpro.frag"));
	program.addattribute("vertexPosition", Lengine::POS);
	program.addattribute("vertexColor", Lengine::COL);
	program.addattribute("vertexuv", Lengine::UVC);
	program.linkshaders();

	//get texture dim
	glm::vec2 areaDim = getArea();

	float scale = cam.getscale();
	glm::ivec2 screenDim = cam.getScreenDim();

	glm::ivec2 textureDim;
	textureDim.x = ceil(areaDim.x*scale / screenDim.x)*screenDim.x;
	textureDim.y = ceil(areaDim.y*scale / screenDim.y)*screenDim.y;

	//frame buffer prepared
	GLuint frameBuffer;
	GLuint frameTexture;

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureDim.x, textureDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//store onriginal viewport data
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	//render process
	Lengine::SpriteBatch frameBatch;
	frameBatch.init();
	program.use();
	GLint samplerLoc = program.getuniformposition("mysampler");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerLoc, 0);

	GLint camID = program.getuniformposition("P");
	for (int x = 0;x <= screenDim.x;x += screenDim.x) {
		for (int y = 0;y <= screenDim.y;y += screenDim.y) {
			cam.setposition(glm::vec2(x / scale, y / scale));
			cam.change();
			glm::mat4 camMatrix1 = cam.getcameramatrix();
			glUniformMatrix4fv(camID, 1, GL_FALSE, &camMatrix1[0][0]);

			frameBatch.begin(Lengine::GlyphSortType::BACK_TO_FRONT);

			draw(&frameBatch);

			frameBatch.end();

			glViewport(x, y, screenDim.x, screenDim.y);
			frameBatch.renderBatch();
		}
	}

	program.unuse();

	//texture download from opengl
	m_textureData.clear();
	m_texWidth = textureDim.x;
	m_texHeight = textureDim.y;
	m_textureData.resize(textureDim.x * textureDim.y * 4);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_textureData[0]);

	//delete everything and set viewPort back
	glBindBuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(vp[0], vp[1], vp[2], vp[3]);

	glDeleteTextures(1, &frameTexture);
	glDeleteBuffers(1, &frameBuffer);
}

void Tiles::writeAsBinary(std::ofstream& fout) {
	fout.write((char*)&m_texWidth, sizeof(int));
	fout.write((char*)&m_texHeight, sizeof(int));

	int size = m_textureData.size();
	fout.write((char*)&size, sizeof(int));
	fout.write((char*)m_textureData.data(), sizeof(int));
}

void Tiles::readAsBinary(std::ifstream& fin) {
	fin.read((char*)&m_texWidth, sizeof(int));
	fin.read((char*)&m_texHeight, sizeof(int));

	int size = m_textureData.size();
	fin.read((char*)&size, sizeof(int));
	m_textureData.resize(size);
	fin.read((char*)m_textureData.data(), sizeof(int));
}

glm::vec2 Tiles::getArea() {
	float Max_x = 0.0f;
	float Max_y = 0.0f;
	float Min_x = 0.0f;
	float Min_y = 0.0f;
	for (auto& T : m_tiles) {
		float x = T.getPositionX();
		float y = T.getPositionY();
		if (Max_x < x)	Max_x = x;
		else if (Min_x > x)	Min_x = x;
		if (Max_y < y)	Max_y = y;
		else if (Min_y > y)	Min_y = y;
	}
	return glm::vec2(Max_x - Min_x, Max_y - Min_y);
}