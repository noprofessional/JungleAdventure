#pragma once
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
namespace Lengine {
class Camera
{
public:
	Camera();
	~Camera();

	//initialize camera matrix
	void init(int screenWidth, int screenHeight);

	//setters
	void setposition(const glm::vec2&newposition) { _position = newposition;m_needupdate = true; }
	void offsetPosition(const glm::vec2& posDelta) { _position += posDelta, m_needupdate = true; }
	void setscale(float newscale) { _scale = newscale; m_needupdate = true; }
	void offsetScale(float scaleDelta) { _scale += scaleDelta, m_needupdate = true; }

	//getters
	glm::vec2 getposition() { return _position; }
	float getscale() { return _scale; }
	glm::ivec2 getScreenDim() { return glm::ivec2(_screenWidth, _screenHeight); }
	glm::mat4 getcameramatrix() { return _cameraMatrix; }

	//update
	void change();

	//converter
	glm::vec2 wintoworld(glm::vec2 wondowcords);

	//judger
	bool incamera(const glm::vec2& centerposition, const glm::vec2 &width_height);

private:
	int _screenWidth;
	int _screenHeight;
	bool m_needupdate;
	glm::vec2 _position;
	glm::mat4 _cameraMatrix;
	glm::mat4 _orthoMatrix;
	float _scale;

};

}