#pragma once
#include<Lengine/IScreen.h>
#include<Lengine/Camera.h>
#include<Lengine/ShaderPro.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/GUI.h>


class MenuScreen :public Lengine::IScreen
{
public:
	MenuScreen(Lengine::IMainGame* ownergame);
	~MenuScreen();
	void build() override;
	void destroy() override;

	void process() override;
	void update() override;
	void draw() override;
private:

	bool actionEnterGame(const CEGUI::EventArgs& args);
	bool actionExit(const CEGUI::EventArgs& ea);
	bool actionEdit(const CEGUI::EventArgs& ea);

	Lengine::GUI m_gui;
	//Lengine::Camera m_camera;
	//Lengine::ShaderPro m_program;
	//Lengine::SpriteBatch m_spriteBatch;


};

