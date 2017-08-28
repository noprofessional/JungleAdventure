#include "Camera.h"

namespace Lengine {
Camera::Camera() :_screenWidth(0),_screenHeight(0),m_needupdate(false)
,_position(0.0f, 0.0f),_cameraMatrix(1.0f),_orthoMatrix(1.0f),_scale(1.0f)
{

}
Camera::~Camera()
{
}

void Camera::init(int screenWidth, int screenHeight) {
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	//ortho matrix init with left, right, bottom, up
	_orthoMatrix = glm::ortho(0.0f, float(_screenWidth), 0.0f, float(_screenHeight));
	glm::vec3 translate(-_position.x + _screenWidth / 2, -_position.y + _screenHeight / 2, 0.0f);
	_cameraMatrix = glm::translate(_orthoMatrix, translate);
}

void Camera::change() {
	if (m_needupdate)
	{
		//translate vec3 to move&scale matrix
		glm::vec3 translate(-_position.x + _screenWidth / 2, -_position.y + _screenHeight / 2, 0.0f);
		_cameraMatrix = glm::translate(_orthoMatrix, translate);
		//when scale use cameraMatrix not the ortho one,OR POSITION DOESN'T CHANGE!!
		glm::vec3 scale(_scale, _scale, 0.0f);
		_cameraMatrix = glm::scale(glm::mat4(1.0f), scale)*_cameraMatrix;
		m_needupdate = false;
	}
}

glm::vec2 Camera::wintoworld(glm::vec2 windowcords) {
	windowcords.y = _screenHeight - windowcords.y;
	windowcords -= glm::vec2(_screenWidth / 2, _screenHeight / 2);
	windowcords /= _scale;
	windowcords += _position;
	return windowcords;
}

bool Camera::incamera(const glm::vec2 &centerposition, const glm::vec2 &width_height)
{
	glm::vec2 screenwidth_height(_screenWidth, _screenHeight);
	//the biger scale is, the small camera box became
	glm::vec2 scaledwidth_height = screenwidth_height / _scale;

	//AABB collision test
	glm::vec2 direcVector(centerposition - _position);
	const float X_minDistance = scaledwidth_height.x / 2 + width_height.x / 2;
	const float Y_minDistance = scaledwidth_height.y / 2 + width_height.y / 2;

	//the distance beteewn two objects' edges
	float X_space = abs(direcVector.x) - X_minDistance;
	float Y_space = abs(direcVector.y) - Y_minDistance;

	//if x or y has extra space they are not touching
	if (X_space > 0 || Y_space > 0)
		return false;
	return true;

}

}