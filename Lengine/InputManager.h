#pragma once
#include <unordered_map>
#include <glm\glm.hpp>

namespace Lengine {
class InputManager
{
public:
	InputManager();
	~InputManager();

	void presskey(unsigned int keyID);
	void releasekey(unsigned int keyID);

	bool isKEYdown(unsigned int keyID);
	bool isKEYpressed(unsigned int keyID);
	bool isKEYdoubleClicked(unsigned int keyID);
	void update();

	void setmousecords(int x, int y);
	const glm::vec2& getmousecords() { return m_mousePos; }
private:
	bool wasKEYdown(unsigned int keyID);
	std::unordered_map<unsigned int, bool> _previouskeyMap;
	std::unordered_map<unsigned int, bool> _keyMap;
	std::unordered_map<unsigned int, unsigned int> m_timeKeyMap;
	glm::vec2 m_mousePos;
	int frameCount=100;
};

}
