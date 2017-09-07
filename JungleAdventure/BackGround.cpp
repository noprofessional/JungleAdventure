#include "BackGround.h"
#include <random>
#include <Lengine/FileIO.h>
#include <iostream>
BackGround::BackGround()
{
}

BackGround::BackGround(Lengine::ColorRGBA8 color, int cloudCount) {
	m_color = color;
	m_clouds.resize(cloudCount);

	std::vector<Lengine::DivEntry> cloudFiles;
	Lengine::FileIO::getDirectoryEntries("Textures/Background/Cloud", cloudFiles);
	for (auto&C : cloudFiles) {
		if (!C.isDirectory) {
			m_cloudTextures.push_back(Lengine::textureCache.gettexture(C.path));
		}
	}

	static std::default_random_engine randCore;
	static std::uniform_real_distribution<float> RelativeXGen(-1.2f, 1.2f);
	static std::uniform_real_distribution<float> RelativeYGen(-0.1f, 0.5f);
	static std::uniform_real_distribution<float> sizeGen(0.5, 1.2f);
	static std::uniform_int_distribution<int> intGen(0, m_cloudTextures.size()-1);

	for (int i = 0;i < cloudCount;i++) {
		m_clouds[i].DestRect.x = RelativeXGen(randCore);
		m_clouds[i].DestRect.y = RelativeYGen(randCore);
		m_clouds[i].DestRect.w = m_clouds[i].DestRect.z = sizeGen(randCore);
		m_clouds[i].texure = m_cloudTextures[intGen(randCore)];
	}

}

BackGround::~BackGround()
{
}

void BackGround::setAsCurrent(Lengine::Window* window) {

	window->setcolor(m_color.r / 255.0f, m_color.g / 255.0f, m_color.b / 255.0f, m_color.a / 255.0f);

	int m_screenWidth = window->getscreenwidth();
	int screenHeight = window->getscreenheight();

	m_cameraXPosMAX = m_screenWidth*3.0f / 2.0f;

	m_camera.init(m_screenWidth, screenHeight);
	m_camera.setposition(glm::vec2(m_cameraXPosMAX, 0.0f));
	m_camera.setscale(1.0f);

	for (auto&C : m_clouds) {
		C.DestRect.x *= m_screenWidth / 2.0f;
		C.DestRect.y *= screenHeight ;
		C.DestRect.z *= C.texure->width;
		C.DestRect.w *= C.texure->height;
	}
	printf("pos %f, %f, %f ,%f\n", m_clouds[0].DestRect.x, m_clouds[0].DestRect.y, m_clouds[0].DestRect.z, m_clouds[0].DestRect.w);

}
void BackGround::update() {
	
	m_camera.offsetPosition(glm::vec2(-1.0f, 0.0f));
	if (m_camera.getposition().x <= -m_cameraXPosMAX)
		m_camera.setposition(glm::vec2(m_cameraXPosMAX, 0.0f));
	m_camera.change();
}

void BackGround::draw(Lengine::ShaderPro* shaderProgram, Lengine::SpriteBatch* spriteBatch) {
	shaderProgram->use();

	GLint samplerLoc = shaderProgram->getuniformposition("mysampler");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerLoc, 0);

	GLint pLoc = shaderProgram->getuniformposition("P");
	glUniformMatrix4fv(pLoc, 1, false, &(m_camera.getcameramatrix()[0][0]));

	spriteBatch->begin();

	for (auto& C:m_clouds) {
		spriteBatch->draw(C.DestRect,glm::vec4(0.0f,0.0f,1.0f,1.0f),C.texure->ids[0],-1.0f);
	}
	spriteBatch->end();

	spriteBatch->renderBatch();

	shaderProgram->unuse();
}
