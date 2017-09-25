#include "IScreen.h"
#include "IMainGame.h"

namespace Lengine{

IScreen::IScreen(IMainGame* ownergame,int screenIdentifier)
{
	m_game = ownergame;
	m_screenIndex = screenIdentifier;
}

IScreen::~IScreen()
{
	//empty
}

void IScreen::onEntry()
{
	m_state = ScreenState::RUNNING;
	build();
}

void IScreen::onExit() {
	m_state = ScreenState::NONE_STATE;
	destroy();
}

void IScreen::loop() {
	process();
	update();
}

void IScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		basicInputProcess(evnt);
	}
}

void IScreen::basicInputProcess(SDL_Event& evnt) {
	InputManager* temp = m_game->getInputManager();
	switch (evnt.type) {
	case SDL_QUIT:
		m_game->setGameState(GameState::EXIT);
		break;
	case SDL_KEYDOWN:
		temp->presskey(evnt.key.keysym.sym);
		break;
	case SDL_KEYUP:
		temp->releasekey(evnt.key.keysym.sym);
		break;
	case SDL_MOUSEMOTION:
		temp->setmousecords(evnt.motion.x, evnt.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		temp->presskey(evnt.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		temp->releasekey(evnt.button.button);
		break;
	}
}
}