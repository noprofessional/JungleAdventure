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
	else return false;
}

void InputManager::update() {
	//copy keymap to previous key map
	for (auto it : _keyMap) {
		_previouskeyMap[it.first] = it.second;
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