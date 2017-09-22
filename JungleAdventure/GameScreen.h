#pragma once
#include<memory>
#include<Lengine/IMainGame.h>
#include<Lengine/IScreen.h>
#include<Lengine/Camera.h>
#include<Lengine/ShaderPro.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/DebugRender.h>

#include "Player.h"
#include "Box.h"
#include "Light.h"
#include "BackGround.h"
class GameScreen:public Lengine::IScreen
{
public:
	GameScreen(Lengine::IMainGame* ownergame);
	~GameScreen();
	void build() override;
	void destroy() override;

	void process() override;
	void update() override;
	void draw() override;
private:
	void cameraTrack();
	Lengine::Camera m_camera;
	Lengine::ShaderPro m_program;
	Lengine::ShaderPro m_lightPro;
	Lengine::SpriteBatch m_spriteBatch;
	Lengine::SpriteBatch m_lightBatch;
	Lengine::DebugRender m_debugRender;

	std::unique_ptr<b2World> m_world;
	Player m_player;
	std::vector<Box> m_boxes;
	std::vector<Light> m_lights;
	BackGround m_background;

	std::vector<GLubyte> buffer;
	bool capture;

	Lengine::GLtexture* frameTexture;

	bool m_debuging = false;
};

