#include"IMainGame.h"
#include"IScreen.h"
#include"FatalError.h"
#include"GameEngine.h"
#include<iostream>
namespace Lengine {
	IMainGame::IMainGame() {
		//empty
	}
	IMainGame::~IMainGame() {
		//empty
	}

	void IMainGame::run() {
		init();
		check();
		gameloop();
	}

	void IMainGame::check() {
		if (GameEngine::engineInit == false)
			fatalError("Haven't initialize GameEngine!");
		printf("MaxFPS is set to %f\nTargetFPS is set to %f\n", m_fpsLimiter.getMaxFPS(), m_fpsLimiter.getTargetFPS());
		printf("Window has width of %d and height of %d\n", m_window.getscreenwidth(), m_window.getscreenheight());
		printf("There are %d screen in the screen list\n", m_list.getListSize());
	}

	void IMainGame::gameloop() {
			while (m_gamestate == GameState::PLAY) {
				//TODO: change to use delta time step

				//update inputManager
				m_inputManager.update();

				//update screen and get current screen
				m_screenptr = m_list.update();

				//draw the current screen
				m_window.clean();
				m_screenptr->draw();
				m_window.swapbuffer();

				//limit fps
				m_fpsLimiter.limit();

			}
	}
}