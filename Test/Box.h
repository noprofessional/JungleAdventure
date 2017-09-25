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

	void debugDraw(Lengine::DebugRender * debugRenderer, bool selected);
	void addToWorld(b2World* world);

	bool inBox(const glm::vec2& mouseCords);

	void writeAsText(std::ofstream& fout)const ;
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout)const;
	void readFromBinary(std::ifstream& fin);

private:
	glm::vec4 m_desRec;
	float m_angle;
	PhysicalMode m_physicalMode;

	b2Body* m_body;
};
class Boxes {
	void addBox(Box& box);
	void selectBox(const glm::vec2& mouseCords);
	void deleteBox(const glm::vec2& mouseCords);
	void debugDraw(Lengine::DebugRender* debugRender,bool debugging);
	void addToWorld(b2World* world);

	void writeAsText(std::ofstream& fout);
	void readFromText(std::ifstream& fin);

	void writeAsBinary(std::ofstream& fout);
	void readFromBinary(std::ifstream& fin);
private:
	std::vector<Box> m_boxes;
	int m_selectedBoxIndex;
};