#pragma once
#include <Lengine/SpriteBatch.h>
#include <Lengine/DebugRender.h>
#include <Lengine/InputManager.h>
#include <Lengine/Camera.h>
#include <glm/glm.hpp>
#include <string>

class TextureUVselector
{
public:
	enum AlignMode { LEFTTOP, LEFTBOTTOM, RIGHTTOP, RIGHTBOTTOM, CENTER };

	TextureUVselector();
	~TextureUVselector();

	void init(glm::vec2 pos,AlignMode alignMode);

	void updateTexture(
			Lengine::GLtexture* texture,
			glm::vec2 gridDIM = glm::vec2(32.0f, 32.0f));
	
	void drawTexture(Lengine::SpriteBatch* spriteBatch);

	void drawLines(Lengine::DebugRender* debugRender);

	bool isInSelector(const glm::vec2& mouseCords);

	void updateUV(const glm::vec2& mouseCords);

	const glm::vec4& getUV() { return m_uv; }
	void setUV(const glm::vec4& uv) { m_uv = uv; }
private:
	glm::vec2 m_selectorPos;
	AlignMode m_alignMode;

	Lengine::GLtexture* m_texture;
	glm::vec2 m_texturePos;
	glm::vec2 m_textureDim;
	glm::vec2 m_gridDIM;

	Lengine::InputManager* m_inputManager;
	glm::vec4 m_uv;
};

