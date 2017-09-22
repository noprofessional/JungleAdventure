#include "TextureUVselector.h"

bool isInBox(const glm::vec2& pos, const glm::vec4& boxArea);

TextureUVselector::TextureUVselector()
{
}


TextureUVselector::~TextureUVselector()
{
}

void TextureUVselector::init(glm::vec2 Pos, AlignMode AlignMode) {
	m_selectorPos = Pos;
	m_alignMode = AlignMode;
}

void TextureUVselector::updateTexture(
		Lengine::GLtexture* texture,
		glm::vec2 gridDIM/* = glm::vec2(32.0f, 32.0f)*/)
{
	if (m_texture != texture) {
		m_texture = texture;
		float HalfWidth = m_texture->width / 2.0f;
		float HalfHeight = m_texture->height / 2.0f;
		switch (m_alignMode)
		{
		case LEFTTOP:
			m_texturePos = m_selectorPos + glm::vec2(HalfWidth, -HalfHeight);
			break;
		case LEFTBOTTOM:
			m_texturePos = m_selectorPos + glm::vec2(HalfWidth, HalfHeight);
			break;
		case RIGHTTOP:
			m_texturePos = m_selectorPos + glm::vec2(-HalfWidth, -HalfHeight);
			break;
		case RIGHTBOTTOM:
			m_texturePos = m_selectorPos + glm::vec2(-HalfWidth, HalfHeight);
			break;
		case CENTER:
			m_texturePos = m_selectorPos;
			break;
		}
		m_textureDim = glm::vec2(m_texture->width, m_texture->height);
		m_uv = glm::vec4(0.0f);
	}
	m_gridDIM = gridDIM;
}

void TextureUVselector::drawTexture(Lengine::SpriteBatch* spriteBatch)
{
	if (m_texture) {
		spriteBatch->draw(glm::vec4(m_texturePos, m_textureDim), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture->ids[0], 1.0f);
	}
}

void TextureUVselector::drawLines(Lengine::DebugRender* debugRender) {
	if (m_texture) {
		debugRender->drawGrid(glm::vec4(m_texturePos, m_textureDim), m_gridDIM);

		glm::vec2 boxPosOffset(
			(m_uv.x + m_uv.z / 2.0f - 0.5f)*m_texture->width,
			(m_uv.y + m_uv.w / 2.0f - 0.5f)*m_texture->height);
		glm::vec4 boxDesRect(boxPosOffset + m_texturePos, m_gridDIM);
		debugRender->drawBox(boxDesRect, Lengine::ColorRGBA8(255, 0, 255, 255), 0.0f);
	}
}

bool TextureUVselector::isInSelector(const glm::vec2& mouseCords) {
	if (isInBox(mouseCords, glm::vec4(m_texturePos, m_textureDim)))
		return true;
	else
		return false;
}

void TextureUVselector::updateUV(const glm::vec2& mouseCords) {
	if (isInSelector(mouseCords)) {

		glm::vec2 desVec = mouseCords - m_texturePos + m_textureDim / 2.0f;
		float nx = m_textureDim.x / m_gridDIM.x;
		float ny = m_textureDim.y / m_gridDIM.y;

		m_uv.x = floor(desVec.x / m_gridDIM.x) / nx;
		m_uv.y = floor(desVec.y / m_gridDIM.y) / ny;
		m_uv.z = 1 / nx;
		m_uv.w = 1 / ny;
	}
}

bool isInBox(const glm::vec2& pos,const glm::vec4& boxArea) {
	glm::vec2 desVec(pos.x - boxArea.x, pos.y - boxArea.y);
	if (abs(desVec.x) < boxArea.z/2.0f && abs(desVec.y) < boxArea.w/2.0f) {
		return true;
	}
	else {
		return false;
	}
}

//glm::vec4 converToDesRec(const glm::vec4& uv) {
//	glm::mat4(
//		1.0f,0.0f,0.0f,0.0f,
//		0.0f,1.0f,0.0f,0.0f,
//		0.5f,0.0f,m_grid.x,0.0f,
//		
//	)
//}