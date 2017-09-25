#include "Box.h"
#include <fstream>
const float PI = 3.1415926f;
const float HALFCIRCLE = 180.0f;
//box
Box::Box() {

}

Box::Box(glm::vec4 DesRec, float Angle, PhysicalMode physicalMode) {
	m_desRec = DesRec;
	m_angle = Angle;
	m_physicalMode = physicalMode;
}

Box::~Box() {

}

void Box::debugDraw(Lengine::DebugRender * debugRenderer,bool selected /*= false*/) {
	if (!selected) {
		switch (m_physicalMode)
		{
		case PhysicalMode::RIGID:
			debugRenderer->drawBox(m_desRec, Lengine::ColorRGBA8(255, 0, 0, 255), m_angle);
			break;
		case PhysicalMode::DYNAMIC:
			debugRenderer->drawBox(m_desRec, Lengine::ColorRGBA8(0, 255, 0, 255), m_angle);
			break;
		case PhysicalMode::MOVABLE:
			debugRenderer->drawBox(m_desRec, Lengine::ColorRGBA8(255, 0, 255, 255), m_angle);
			break;
		}
	}
	else {
		debugRenderer->drawBox(m_desRec, Lengine::ColorRGBA8(255, 255, 0, 255), m_angle);
	}
}

void Box::addToWorld(b2World* world) {

	b2BodyDef bodydef;
	switch (m_physicalMode)
	{
	case PhysicalMode::RIGID:
		bodydef.type = b2_staticBody;
		break;
	case PhysicalMode::DYNAMIC:
		bodydef.type = b2_dynamicBody;
		break;
	case PhysicalMode::MOVABLE:
		bodydef.type = b2_kinematicBody;
		break;
	}
	bodydef.position.Set(m_desRec.x, m_desRec.y);
	bodydef.angle = m_angle*PI/HALFCIRCLE;
	m_body = world->CreateBody(&bodydef);

	b2PolygonShape shape;
	shape.SetAsBox(m_desRec.z / 2.0f, m_desRec.w / 2.0f);
	b2Fixture* fixture = m_body->CreateFixture(&shape, 1.0f);

}

bool Box::inBox(const glm::vec2& mouseCords) {
	glm::vec2 desVec;
	desVec.x = mouseCords.x - m_desRec.x;
	desVec.y = mouseCords.y - m_desRec.y;
	if (abs(desVec.x) < m_desRec.z / 2.0f && abs(desVec.y) < m_desRec.w / 2.0f)
		return true;
	else
		return false;
}

void Box::writeAsText(std::ofstream& fout)const {
	fout << m_desRec.x << ' ' << m_desRec.y << ' ' << m_desRec.z << ' ' << m_desRec.w << ' '
		<< m_angle << ' '
		<< m_physicalMode << '\n';
}

void Box::readFromText(std::ifstream& fin) {
	int physicalModeCode;
	fin >> m_desRec.x >> m_desRec.y >> m_desRec.z >> m_desRec.w
		>> m_angle
		>> physicalModeCode;
	m_physicalMode = PhysicalMode(physicalModeCode);
}

void Box::writeAsBinary(std::ofstream& fout)const {
	fout.write((char*)&m_desRec, sizeof(glm::vec4));
	fout.write((char*)&m_angle, sizeof(float));
	fout.write((char*)&m_physicalMode, sizeof(m_physicalMode));
}

void Box::readFromBinary(std::ifstream& fin) {
	fin.read((char*)&m_desRec, sizeof(glm::vec4));
	fin.read((char*)&m_angle, sizeof(float));
	fin.read((char*)&m_physicalMode, sizeof(m_physicalMode));
}

void Box::showInfo(glm::vec4& DesRec, float& Angle, PhysicalMode& physicalMode) {
	DesRec = m_desRec;
	Angle = m_angle;
	physicalMode = m_physicalMode;
}


//boxes
Boxes::Boxes() {
	m_selectedBoxIndex = -1;
}

void Boxes::addBox(Box& box) {
	m_boxes.push_back(box);
}

bool Boxes::selectBox(const glm::vec2& mouseCords) {
	m_selectedBoxIndex = -1;
	for (int i = 0;i < m_boxes.size();i++) {
		if (m_boxes[i].inBox(mouseCords)){
			m_selectedBoxIndex = i;
		}
	}
	return hasSelection();
}

void Boxes::deleteBox(const glm::vec2& mouseCords) {
	for (int i = 0;i < m_boxes.size();i++) {
		if (m_boxes[i].inBox(mouseCords)) {
			m_boxes[i] = m_boxes.back();
			m_boxes.pop_back();
			break;
		}
	}
	m_selectedBoxIndex = -1;
}

void Boxes::debugDraw(Lengine::DebugRender* debugRender) {

	for (auto& B : m_boxes) {
		B.debugDraw(debugRender);
	}

	if (m_selectedBoxIndex != -1) {
		m_boxes[m_selectedBoxIndex].debugDraw(debugRender, true);
	}
}

void Boxes::addToWorld(b2World* world) {
	for (auto& B : m_boxes) {
		B.addToWorld(world);
	}
}

void Boxes::writeAsText(std::ofstream& fout)const {
	fout << m_boxes.size()<<'\n';
	for (auto& B : m_boxes) {
		B.writeAsText(fout);
	}
}

void Boxes::readFromText(std::ifstream& fin) {
	int size;
	fin >> size;
	m_boxes.resize(size);
	for (auto& B : m_boxes) {
		B.readFromText(fin);
	}
}

void Boxes::writeAsBinary(std::ofstream& fout)const {
	int size = m_boxes.size();
	fout.write((char*)&size, sizeof(int));
	for (auto& B : m_boxes) {
		B.writeAsBinary(fout);
	}
}

void Boxes::readFromBinary(std::ifstream& fin) {
	int size;
	fin.read((char*)&size, sizeof(int));
	m_boxes.resize(size);
	for (auto& B : m_boxes) {
		B.readFromBinary(fin);
	}
}

void Boxes::showSelectedInfo(glm::vec4& DesRec, float& Angle, PhysicalMode& physicalMode) {
	if (m_selectedBoxIndex != -1) {
		m_boxes.at(m_selectedBoxIndex).showInfo(DesRec, Angle, physicalMode);
	}
}

void Boxes::offsetSelectedPos(const float& xOffset, const float& yOffset) {
	if (m_selectedBoxIndex != -1) {
		glm::vec4 desRec;
		float angle;
		PhysicalMode physicalMode;
		m_boxes[m_selectedBoxIndex].showInfo(desRec, angle, physicalMode);
		desRec.x += xOffset;
		desRec.y += yOffset;
		m_boxes[m_selectedBoxIndex] = Box(desRec, angle, physicalMode);
	}
}

void Boxes::replaceSelected(Box& box) {
	m_boxes[m_selectedBoxIndex] = box;
}
