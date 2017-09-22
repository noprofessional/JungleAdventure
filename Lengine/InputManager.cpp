#include "InputManager.h"
#include<iostream>
namespace Lengine {


InputManager::InputManager() :m_mousePos(0.0f)
{
}


InputManager::~InputManager()
{
}

void InputManager::presskey(unsigned int keyID) {
	_keyMap[keyID] = true;
}
void InputManager::releasekey(unsigned int keyID) {
	_keyMap[keyID] = false;
	m_timeKeyMap[keyID] = frameCount;
}
bool InputManager::isKEYdown(unsigned int keyID) {
	auto it = _keyMap.find(keyID);
	if (it != _keyMap.end())
		return it->second; 
	else
		return false;
}

//return true if key was just pressed
bool InputManager::isKEYpressed(unsigned int keyID) {
	if (isKEYdown(keyID) && wasKEYdown(keyID) != true)
		return true;
	else 
		return false;
}

bool InputManager::isKEYdoubleClicked(unsigned int keyID) {
	if (isKEYpressed(keyID)&& frameCount-m_timeKeyMap[keyID]< 10)
		return true;
	else
		return false;
}

void InputManager::update() {
	//copy keymap to previous key map
	for (auto it : _keyMap) {
		_previouskeyMap[it.first] = it.second;
	}

	frameCount++;
	if (frameCount > 100000)
	{
		frameCount = 100;
		for (auto& it : m_timeKeyMap) {
			it.second = 50;
		}
	}
}

void InputManager::setmousecords(int x, int y) {
	m_mousePos.x = x;
	m_mousePos.y = y;
}

bool InputManager::wasKEYdown(unsigned int keyID) {
	auto it = _previouskeyMap.find(keyID);
	if (it != _previouskeyMap.end())
		return it->second;
	else
		return false;
}
}