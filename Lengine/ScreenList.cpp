#include "ScreenList.h"
#include "IScreen.h"
#include"Fatalerror.h"
#include<string>
namespace Lengine{

	ScreenList::ScreenList()
	{
	}

	ScreenList::~ScreenList()
	{
		for (int i = 0; i < m_screenlist.size();i++) {
			if (m_screenlist[i] != nullptr) {
				//in case delete nullptr
				delete m_screenlist[i];
			}
		}
		m_screenlist.clear();
	}

	IScreen*  ScreenList::update()
	{
		IScreen* p = getCurrentScreen();
		switch (p->m_state)
		{
		case ScreenState::RUNNING:
			p->loop();
			break;
		case ScreenState::TO_NEXT:
			p->onExit();
			p = getNextScreen();
			p->onEntry();
			break;
		case ScreenState::TO_PREVIOUS:
			p->onExit();
			p=getPreviousScreen();
			p->onEntry();
			break;
		default:
			break;
		}
		return p;
	}

	void ScreenList::addScreen(IScreen* screen)
	{
		if (screen != nullptr) {
			//only add valid screen to the screen
			if (m_screenlist.size() <= screen->getScreenIndex()) {
				//resize when index is out of range
				//means some member can be nullptr
				m_screenlist.resize(screen->getScreenIndex() + 1);
			}
			m_screenlist[screen->getScreenIndex()] = screen;
		}
		else {
			fatalError("Screen pointer is null!\n");
		}

	}
	void ScreenList::setScreen(int screenIndex) {
		if (m_currentscreenindex != -1) {
			//if there is a current screen exit it
			getCurrentScreen()->onExit();
		}
		//set current screen index
		m_currentscreenindex = screenIndex;
		//Entry the required screen
		getCurrentScreen()->onEntry();

	}

	IScreen*  ScreenList::getCurrentScreen()
	{
		if (m_currentscreenindex < m_screenlist.size()) {
			return m_screenlist[m_currentscreenindex];
		}
		else {
			fatalError("Screen Index OUT OF RANGE!");
		}
	}
	IScreen* ScreenList::getNextScreen() {
		int nextIndex = getCurrentScreen()->getNextScreenIndex();
		if (nextIndex != -1) {
			//if screen index set correctly
			m_currentscreenindex = nextIndex;
			IScreen* p = getCurrentScreen();
			if (p != nullptr) {
				return m_screenlist[nextIndex];
			}
			else {
				fatalError("There is no next screen!");
			}
		}
		else {
			fatalError("Next Screen Index invalid!");
		}
	}
	IScreen* ScreenList::getPreviousScreen() {
		int preIndex = getCurrentScreen()->getPreviousScreenIndex();
		if (preIndex != -1) {
			//if screen index set correctly
			m_currentscreenindex = preIndex;
			IScreen* p = getCurrentScreen();
			if (p!= nullptr) {
				return p;
			}
			else {
				fatalError("There is no Previous screen!");
			}
		}
		else {
			fatalError("Previous Screen Index invalid!");
		}
	}

	
}