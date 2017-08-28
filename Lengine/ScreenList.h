#pragma once
#include<vector>
namespace Lengine {
	class IScreen;
	class ScreenList
	{
	public:
		ScreenList();
		~ScreenList();
		//update and return the current screen pointer
		IScreen* update();
		//add screen to the list
		void addScreen(IScreen* screen);
		void setScreen(int screenIndex);
		//----getter----
		int getListSize() { return m_screenlist.size(); }
	private:
		IScreen* getCurrentScreen();
		IScreen* getNextScreen();
		IScreen* getPreviousScreen();
		std::vector<IScreen*> m_screenlist;
		int m_currentscreenindex = -1;
	};

}