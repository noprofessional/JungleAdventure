#pragma once
#include <Lengine/Vertex.h>
#include <glm/glm.hpp>
#include <Lengine/TextureCache.h>
#include <vector>
#include <Lengine/Window.h>
#include <Lengine/Camera.h>
#include <Lengine/SpriteBatch.h>
#include <Lengine/ShaderPro.h>

struct cloud{
	glm::vec4 DestRect;
	Lengine::GLtexture* texure;
};

class BackGround
{
public:
	BackGround();
	BackGround(Lengine::ColorRGBA8 color,int cloudCount);
	~BackGround();
	void setAsCurrent(Lengine::Window* window);
	void update();
	void draw(Lengine::ShaderPro* shaderProgram, Lengine::SpriteBatch* spriteBatch);
private:
	Lengine::ColorRGBA8 m_color;
	std::vector<cloud> m_clouds;
	std::vector<Lengine::GLtexture*> m_cloudTextures;
	int m_cameraXPosMAX;
	Lengine::Camera m_camera;

};

