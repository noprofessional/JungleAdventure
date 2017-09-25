#include "Box.h"
#include <fstream>
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

void Box::debugDraw(Lengine::DebugRender * debugRenderer,bool selected) {
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
	bodydef.angle = m_angle;
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


//boxes
void Boxes::addBox(Box& box) {
	m_boxes.push_back(box);
}

void Boxes::selectBox(const glm::vec2& mouseCords) {
	m_selectedBoxIndex = -1;
	for (int i = 0;i < m_boxes.size();i++) {
		if (m_boxes[i].inBox(mouseCords)){
			m_selectedBoxIndex = i;
		}
	}
}

void Boxes::deleteBox(const glm::vec2& mouseCords) {
	for (auto& B : m_boxes) {
		if (B.inBox(mouseCords)) {
			B = m_boxes.back();
			m_boxes.pop_back();
		}
	}
	m_selectedBoxIndex = -1;
}

void Boxes::debugDraw(Lengine::DebugRender* debugRender,bool debugging) {
	if (debugging) {
		for (auto& B : m_boxes) {
			B.debugDraw(debugRender, false);
		}
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

void Boxes::writeAsText(std::ofstream& fout) {
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

void Boxes::writeAsBinary(std::ofstream& fout) {
	int size = m_boxes.size();
	fout.write((char*)size, sizeof(int));
	for (auto& B : m_boxes) {
		B.writeAsBinary(fout);
	}
}

void Boxes::readFromBinary(std::ifstream& fin) {
	int size;
	fin.read((char*)size, sizeof(int));
	m_boxes.resize(size);
	for (auto& B : m_boxes) {
		B.readFromBinary(fin);
	}
}