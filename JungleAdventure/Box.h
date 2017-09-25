#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <Lengine/DebugRender.h>
enum PhysicalMode {
	RIGID,
	DYNAMIC,
	MOVABLE
};
class Box {
public:
	Box();
	Box(glm::vec4 DesRec, float Angle, PhysicalMode physicalMode);
	~Box();

	void debugDraw(Lengine::DebugRender * debugRenderer, bool selected = false);
	void addToWorld(b2World* world);

	bool inBox(const glm::vec2& mouseCords);

	void writeAsText(std::ofstream& fout)const ;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const;
	void readFromBinary(std::ifstream& fin);

	void showInfo(glm::vec4& DesRec, float& Angle, PhysicalMode& physicalMode);

private:
	glm::vec4 m_desRec;
	float m_angle;
	PhysicalMode m_physicalMode;

	b2Body* m_body;
};
class Boxes {
public:
	Boxes();
	void addBox(Box& box);
	bool selectBox(const glm::vec2& mouseCords);
	void deleteBox(const glm::vec2& mouseCords);
	void debugDraw(Lengine::DebugRender* debugRender);
	void addToWorld(b2World* world);

	void writeAsText(std::ofstream& fout)const;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const;
	void readFromBinary(std::ifstream& fin);

	void clearSelection() { m_selectedBoxIndex = -1; }
	void showSelectedInfo(glm::vec4& DesRec, float& Angle, PhysicalMode& physicalMode);
	void offsetSelectedPos(const float& xOffset, const float& yOffset);
	void replaceSelected(Box& box);
private:
	bool hasSelection() { return m_selectedBoxIndex != -1; }

	std::vector<Box> m_boxes;
	int m_selectedBoxIndex;
};