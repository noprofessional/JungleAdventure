#pragma once
#include<memory>
#include<Lengine/IMainGame.h>
#include<Lengine/IScreen.h>
#include<Lengine/Camera.h>
#include<Lengine/ShaderPro.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/GUI.h>
#include "Player.h"
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
	bool actionExit(const CEGUI::EventArgs& args);

	Lengine::Camera m_camera;
	Lengine::ShaderPro m_program;
	Lengine::SpriteBatch m_spriteBatch;
	Lengine::DebugRender m_debugRender;
	Lengine::GUI m_gui;
	CEGUI::PushButton* m_pushButton;

	std::unique_ptr<b2World>m_world;
	Player m_player;
};

