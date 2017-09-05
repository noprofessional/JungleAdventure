#pragma once
#include<vector>
#include <map>
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
		//set start screen
		void setStartScreen(int screenIndex);
		//----getter----
		int getListSize() { return m_screenMap.size(); }
	private:
		IScreen* getCurrentScreen();
		IScreen * findScreen(int ScreenIndex);

		std::vector<IScreen*> m_screenlist;
		std::map<int, IScreen*> m_screenMap;
		int m_currentscreenindex = -1;
	};

}