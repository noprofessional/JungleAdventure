#include "MenuScreen.h"
#include"ScreenIdentifier.h"
#include<Lengine/IMainGame.h>
#include<iostream>


MenuScreen::MenuScreen(Lengine::IMainGame* ownergame):IScreen(ownergame,MENU_SCREEN)
{
	//empty
}
MenuScreen::~MenuScreen()
{
	destroy();
}

void MenuScreen::build() {
	m_game->getWindowPtr()->setcolor(0.0f, 0.0f, 0.3f, 1.0f);

	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.loadScheme("AlfiskoSkin.scheme");
	m_gui.setFont("DejaVuSans-10");

	CEGUI::PushButton* testButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("AlfiskoSkin/Button", 
		glm::vec4(0.5f, 0.5f, 0.0f, 0.0f), glm::vec4(0.1f, 0.05f, 0.0f, 0.0f), "NewGameButton"));
	testButton->setText("New Game");
	//subscribe the Action Enter Game to the button
	testButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScreen::actionEnterGame, this));
	
	CEGUI::PushButton* editButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("AlfiskoSkin/Button", 
		glm::vec4(0.5f, 0.45f, 0.0f, 0.0f), glm::vec4(0.1f, 0.05f, 0.0f, 0.0f), "EditButon"));
	editButton->setText("Level Editor");
	//subscribe the Action Enter Edit Mode to the button
	editButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScreen::actionEdit, this));

	CEGUI::PushButton* testButton2 = static_cast<CEGUI::PushButton*>(m_gui.createWidget("AlfiskoSkin/Button", 
		glm::vec4(0.5f, 0.4f, 0.0f, 0.0f), glm::vec4(0.1f, 0.05f, 0.0f, 0.0f), "ExitButton"));
	testButton2->setText("Exit Game");
	//subscribe the Action Exit Game to the button
	testButton2->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScreen::actionExit, this));

	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);
	
}
void MenuScreen::destroy() {
	m_gui.destroy();
}

void MenuScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_gui.injectEvent(evnt);
		basicInputProcess(evnt);
	}
}
void MenuScreen::update() {
	m_game->getInputManager()->update();
}
void MenuScreen::draw() {
	m_gui.draw();
}

bool MenuScreen::actionEnterGame(const CEGUI::EventArgs& args) {
	m_state = Lengine::ScreenState::TO_NEXT;
	m_nextScreenIndex = GAME_SCREEN;
	return true;
}
bool MenuScreen::actionExit(const CEGUI::EventArgs& ea) {
	m_game->setGameState(Lengine::GameState::EXIT);
	return true;
}
bool MenuScreen::actionEdit(const CEGUI::EventArgs& ea) {
	m_nextScreenIndex = EDIT_SCREEN;
	m_state = Lengine::ScreenState::TO_NEXT;
	return true;
}
