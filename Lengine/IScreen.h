#pragma once
#include<SDL/SDL.h>
namespace Lengine {
	enum class ScreenState { RUNNING, TO_NEXT, TO_PREVIOUS, NONE_STATE };
	class IMainGame;
	class IScreen
	{
	public:
		friend class ScreenList;
		IScreen(IMainGame* ownergame,int screenIdentifier);
		virtual ~IScreen();
		//get call when entry the screen
		//change self state to RUNNING and build
		void onEntry();
		//get call when exit the screen
		//change self state to NONE_STATE
		void onExit();
		//get call when state is RUNNING
		//run the loop
		void loop();

		virtual void build() = 0;
		virtual void destroy() = 0;

		virtual void process();

		virtual void update() = 0;
		virtual void draw() = 0;

		//----getter----
		int getNextScreenIndex() { return m_nextScreenIndex; }
		int getPreviousScreenIndex() { return m_previousScreenIndex; }
		int getScreenIndex() { return m_screenIndex; }

	protected:
		void basicInputProcess(SDL_Event& evnt);
		ScreenState m_state;
		IMainGame* m_game = nullptr;

		int m_nextScreenIndex = -1;
		int m_previousScreenIndex = -1;
		int m_screenIndex=-1;
	};

}