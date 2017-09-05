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
		//for (int i = 0; i < m_screenlist.size();i++) {
		//	if (m_screenlist[i] != nullptr) {
		//		//in case delete nullptr
		//		delete m_screenlist[i];
		//	}
		//}
		//m_screenlist.clear();
		for (int i = 0;i < m_screenMap.size();i++) {
			delete m_screenMap[i];
		}
		m_screenMap.clear();
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
			m_currentscreenindex = p->getNextScreenIndex();
			p = getCurrentScreen();
			p->onEntry();
			break;
		case ScreenState::TO_PREVIOUS:
			p->onExit();
			m_currentscreenindex = p->getPreviousScreenIndex();
			p = getCurrentScreen();
			p->onEntry();
			break;
		default:
			break;
		}
		return p;
	}

	void ScreenList::addScreen(IScreen* screen)
	{
		if (screen!= nullptr) {
			m_screenMap.insert(std::make_pair(screen->getScreenIndex(), screen));
		}
		else {
			fatalError("Screen pointer is null!\n");
		}
		//if (screen != nullptr) {
		//	//only add valid screen to the screen
		//	if (m_screenlist.size() <= screen->getScreenIndex()) {
		//		//resize when index is out of range
		//		//means some member can be nullptr
		//		m_screenlist.resize(screen->getScreenIndex() + 1);
		//	}
		//	m_screenlist[screen->getScreenIndex()] = screen;
		//}
		//else {
		//	fatalError("Screen pointer is null!\n");
		//}

	}

	void ScreenList::setStartScreen(int screenIndex) {
		//if (m_currentscreenindex != -1) {
		//	//if there is a current screen exit it
		//	getCurrentScreen()->onExit();
		//}
		////set current screen index
		//m_currentscreenindex = screenIndex;
		////Entry the required screen
		//getCurrentScreen()->onEntry();

		m_currentscreenindex = screenIndex;
		getCurrentScreen()->onEntry();
	}

	IScreen*  ScreenList::getCurrentScreen()
	{
		return findScreen(m_currentscreenindex);

		//if (m_currentscreenindex < m_screenlist.size()) {
		//	return m_screenlist[m_currentscreenindex];
		//}
		//else {
		//	fatalError("Screen Index OUT OF RANGE!");
		//}
	}

	IScreen* ScreenList::findScreen(int ScreenIndex) {
		auto It = m_screenMap.find(ScreenIndex);
		if (It != m_screenMap.end()) {
			return It->second;
		}
		else {
			fatalError("There is NO screen " + std::to_string(ScreenIndex));
		}
	}
}