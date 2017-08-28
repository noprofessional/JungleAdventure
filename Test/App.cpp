#include "App.h"
#include<Lengine/GameEngine.h>
#include"GameScreen.h"
#include"ScreenIdentifier.h"
const float MAX_FPS = 60.0f;
App::App()
{
}
App::~App()
{
}

void App::init() {
	Lengine::GameEngine::init();

	m_fpsLimiter.setMaxFPS(MAX_FPS);
	m_fpsLimiter.setTargetFPS(MAX_FPS);

	m_window.create("Platform Game", 1000, 700);

	Lengine::IScreen* p = new GameScreen(this);
	m_list.addScreen(p);
	m_list.setScreen(p->getScreenIndex());
}