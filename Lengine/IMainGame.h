#pragma once
#include"ScreenList.h"
#include"Window.h"
#include"InputManager.h"
#include"Timing.h"

namespace Lengine {
	class Iscreen;
	enum class GameState{PLAY,EXIT};
	//interface for MainGame one for each game
	class IMainGame
	{
	public:
		IMainGame();
		virtual ~IMainGame();
		void run();

		Window*			getWindowPtr()						{ return &m_window; }
		InputManager*	getInputManager()					{ return &m_inputManager; }
		void			setGameState(GameState setState)	{ m_gamestate = setState; }

	protected:
		//----override the function and do these:----
		//-init game engine
		//-set fps limiter
		//-create window
		//-create screen and add screen to the list in order
		virtual void init() = 0;
		void check();
		void gameloop();


		ScreenList m_list;
		IScreen* m_screenptr = nullptr;

		GameState m_gamestate=GameState::PLAY;
		FpsLimiter m_fpsLimiter;

		Window m_window;
		InputManager m_inputManager;
	};


}