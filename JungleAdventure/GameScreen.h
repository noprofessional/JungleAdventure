#pragma once
#include<memory>
#include<Lengine/IMainGame.h>
#include<Lengine/IScreen.h>
#include<Lengine/Camera.h>
#include<Lengine/ShaderPro.h>
#include <Lengine/SpriteFont.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/DebugRender.h>
#include<Lengine/GUI.h>

#include "Player.h"
#include "Box.h"
#include "Light.h"
#include "BackGround.h"
#include "Tile.h"
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
	void initUI();
	void cameraTrack();
	Lengine::Camera m_camera;
	Lengine::ShaderPro m_program;
	Lengine::ShaderPro m_lightPro;
	Lengine::SpriteBatch m_spriteBatch;
	Lengine::SpriteBatch m_lightBatch;
	Lengine::DebugRender m_debugRender;
	Lengine::SpriteFont m_fontBatch;

	Lengine::GUI m_gui;
	CEGUI::PushButton* b_back;
	bool onBackClicked(const CEGUI::EventArgs& eas);

	std::unique_ptr<b2World> m_world;
	Player m_player;
	Boxes m_boxes;
	Tiles m_tiles;
	Lights m_lights;
	BackGround m_background;

	bool m_debuging = false;
};

