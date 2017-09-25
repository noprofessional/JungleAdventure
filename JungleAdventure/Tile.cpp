#include "Tile.h"
#include <fstream>
#include <algorithm>
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
	float x = m_desRec.x;
	float y = m_desRec.y;
	float width = m_desRec.z;
	float height = m_desRec.w;
	glm::vec4 leftBottomDesRec(x - width / 2.0f + 0.5f, y - height / 2.0f + 0.5f, 1.0f, 1.0f);

	if (!m_texture->clamped) {
		m_texture = Lengine::textureCache.getSTClampedTexture(m_texture->filePath);
	}
	for (float xCenter = 0.0f;xCenter < width;xCenter++) {
		for (float yCenter = 0.0f;yCenter < height;yCenter++) {
			SpriteBatch->draw(leftBottomDesRec + glm::vec4(xCenter, yCenter, 0.0f, 0.0f), m_uvRec, m_texture->ids[0], m_depth);
		}
	}
}

void Tile::debugDraw(Lengine::DebugRender* dr, bool selected/*= false*/) {
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
	if (abs(desVec.x) < m_desRec.z/2.0f && abs(desVec.y) < m_desRec.w/2.0f)
		return true;
	else
		return false;
}

void Tile::writeAsText(std::ofstream& fout)const {
	fout << m_desRec.x << ' ' << m_desRec.y << ' ' << m_desRec.z << ' ' << m_desRec.w << ' '
		<< m_uvRec.x << ' ' << m_uvRec.y << ' ' << m_uvRec.z << ' ' << m_uvRec.w << ' '
		<< m_texture->filePath.c_str() << ' '
		<< m_depth << '\n';
}

void Tile::readFromText(std::ifstream& fin) {
	std::string texturePath;
	fin >> m_desRec.x >> m_desRec.y >> m_desRec.z >> m_desRec.w
		>> m_uvRec.x >> m_uvRec.y >> m_uvRec.z >> m_uvRec.w
		>> texturePath
		>> m_depth;
	m_texture = Lengine::textureCache.gettexture(texturePath);
}

void Tile::showInfo(glm::vec4& desRec, glm::vec4& uvRec, std::string& texturePath, float& depth)const {
	desRec = m_desRec;
	uvRec = m_uvRec;
	texturePath = m_texture->filePath;
	depth = m_depth;
}

//tiles
void Tiles::addTile(Tile& tile) {
	m_tiles.push_back(tile);
}

bool Tiles::selectTile(const glm::vec2& mouseCords, bool more) {
	
	m_candidateIndex.clear();

	for (int i = 0;i < m_tiles.size();i++) {
		if (m_tiles[i].inTile(mouseCords)) {
			m_candidateIndex.push_back(i);
		}
	}
	
	if (m_candidateIndex.size() > 0) {
		if (more) {
			bool same = false;
			for (int ci =0;ci<m_candidateIndex.size();ci++)
			{
				for (int si = 0;si < m_selectedIndex.size();si++) {
					if (m_candidateIndex[ci] == m_selectedIndex[si]) {
						m_selectedIndex[si] = m_selectedIndex.back();
						m_selectedIndex.pop_back();
						same = true;
					}
				}
				if (!same) {
					m_selectedIndex.push_back(m_candidateIndex[ci]);
				}
			}
		}
		else {
			m_selectedIndex.resize(1);
			m_selectedIndex[0] = m_candidateIndex[0];
		}
	}
	else {
		if (!more)
		{
			m_candidateIndex.clear();
			m_selectedIndex.clear();
		}
	}
	return hasSelection();

}

void Tiles::nextCandidate() {
	if (m_selectedIndex.size() == 1 && m_candidateIndex.size() > 1) {
		int currentCandidate = 0;
		for (int i = 0;i < m_candidateIndex.size();i++) {
			if (m_candidateIndex[i] == m_selectedIndex[0])
				currentCandidate = i;
		}
		int nextCadidate = (currentCandidate + 1) % m_candidateIndex.size();
		m_selectedIndex[0] = m_candidateIndex[nextCadidate];
	}
}

void Tiles::deleteTile(const glm::vec2& mouseCords) {
	selectTile(mouseCords, false);
	if (m_candidateIndex.size() > 0) {
		int topTileIndex = m_candidateIndex[0];
		for (int i = 1;i < m_candidateIndex.size();i++) {
			if (m_tiles[topTileIndex] < m_tiles[m_candidateIndex[i]])
				topTileIndex = m_candidateIndex[i];
		}
		m_tiles[topTileIndex] = m_tiles.back();
		m_tiles.pop_back();

		//clear index info
		//which is changed because the delete op
		m_candidateIndex.clear();
		m_selectedIndex.clear();
	}

}

void Tiles::draw(Lengine::SpriteBatch* sb) {
	for (auto& T : m_tiles) {
		T.draw(sb);
	}
}

void Tiles::drawTexture(Lengine::SpriteBatch* sb,Lengine::Camera* cam) {
	if (m_texture) {
		glm::vec2 textureDim(m_texWidth, m_texHeight);
		glm::vec2 screenDim = cam->getScreenDim();
		glm::vec2 renderPos = (textureDim - screenDim) / 2.0f / cam->getscale();
		glm::vec2 renderDim = textureDim / cam->getscale();
		glm::vec4 uvRec(0.0f, 1.0f, 1.0f, -1.0f);
		sb->draw(glm::vec4(renderPos, renderDim), uvRec, m_texture->ids[0], 0.0f);
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

void Tiles::writeAsText(std::ofstream& fout)const {
	fout << m_tiles.size() << '\n';
	for (auto& T : m_tiles) {
		T.writeAsText(fout);
	}
}

void Tiles::readFromText(std::ifstream& fin) {
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
	float scale = cam.getscale();
	glm::ivec2 screenDim = cam.getScreenDim();

	glm::vec4 areaDim = getArea();
	int leftScreenNum = ceil(abs(areaDim.x)*scale / screenDim.x);
	int rightScreenNum = ceil(abs(areaDim.y)*scale / screenDim.x);
	int downSceenNum = ceil(abs(areaDim.z)*scale / screenDim.y);
	int upScreenNum = ceil(abs(areaDim.w)*scale / screenDim.y);


	glm::ivec2 textureDim;
	textureDim.x = (leftScreenNum + rightScreenNum - 1)*screenDim.x;
	textureDim.y = (downSceenNum + upScreenNum - 1)*screenDim.y;

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
	for (int x = 0;x < textureDim.x;x += screenDim.x) {
		for (int y = 0;y < textureDim.y;y += screenDim.y) {
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
	if (textureDim.x != 0 && textureDim.y != 0) {
		m_textureData.resize(textureDim.x * textureDim.y * 4);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_textureData[0]);
	}

	//delete everything and set viewPort back
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(vp[0], vp[1], vp[2], vp[3]);

	glDeleteTextures(1, &frameTexture);
	glDeleteFramebuffers(1, &frameBuffer);
	program.dispose();
}

void Tiles::writeAsBinary(std::ofstream& fout)const {
	if (m_textureData.size() == 0 && m_tiles.size() > 0)
		Lengine::fatalError("Please render tiles to texture before write to file!");

	fout.write((char*)&m_texWidth, sizeof(int));
	fout.write((char*)&m_texHeight, sizeof(int));

	int size = m_textureData.size();
	fout.write((char*)&size, sizeof(int));
	if (size)
		fout.write((char*)m_textureData.data(), size);
	//else nothing to save
}

void Tiles::readFromBinary(std::ifstream& fin) {
	fin.read((char*)&m_texWidth, sizeof(int));
	fin.read((char*)&m_texHeight, sizeof(int));

	int size = 0;
	fin.read((char*)&size, sizeof(int));
	if (size) {
		m_textureData.resize(size);
		fin.read((char*)m_textureData.data(), size);
		m_texture = Lengine::textureCache.addTexture("Tiles", m_texWidth, m_texHeight, m_textureData);
	}//else nothing to read
}

void Tiles::showSelectedInfo(
		glm::vec4& desRec,
		glm::vec4& uvRec,
		std::string& texturePath,
		float& depth)const
{
	auto& B = m_tiles[m_selectedIndex.back()];
	B.showInfo(desRec, uvRec, texturePath, depth);
}

void Tiles::offsetSelectedPos(const float& xOffset, const float& yOffset) {
	for (auto& i : m_selectedIndex) {
		m_tiles[i].offsetPos(xOffset, yOffset);
	}
}

void Tiles::replaceSelected(Tile& tile) {
	m_tiles[m_selectedIndex.back()] = tile;
}

bool Tiles::hasSelection()const {
	return bool(m_selectedIndex.size());
}

glm::vec4 Tiles::getArea() {
	float Max_x = 0.0f;
	float Max_y = 0.0f;
	float Min_x = 0.0f;
	float Min_y = 0.0f;
	for (auto& T : m_tiles) {

		glm::vec4 desRec = T.getDesRec();
		float left_x = desRec.x - desRec.z / 2.0f;
		float right_x = desRec.x + desRec.z / 2.0f;
		float up_y = desRec.y + desRec.w / 2.0f;
		float down_y = desRec.y - desRec.w / 2.0f;
		Max_x = std::max(Max_x, right_x);
		Min_x = std::min(Min_x, left_x);
		Max_y = std::max(Max_y, up_y);
		Min_y = std::min(Min_y, down_y);
	}
	return glm::vec4(Min_x, Max_x, Min_y, Max_y);
}