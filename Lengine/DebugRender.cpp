#include "DebugRender.h"
#include<string>
#include"FatalError.h"
const char * VERTEX_SRC = R"(
#version 130

in vec2 vertexPosition;
in vec4 vertexColor;
out vec4 fragmentColor;

uniform mat4 P;

void main(){
	gl_Position.xy= (P*vec4(vertexPosition.xy,0.0,1.0)).xy;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	fragmentColor = vertexColor;
}
)";

const char* FRAGMENT_SRC = R"(
#version 130

in vec4 fragmentColor;
out vec4 color;

void main(){
	color =fragmentColor;
}
)";

const float PI = 3.1415926535;

namespace Lengine {

	DebugRender::DebugRender()
	{
		//Empty
	}

	DebugRender::~DebugRender()
	{
		//empty
	}

	void DebugRender::init() {
		//init its own shader program
		m_program.compileShadersFromSource(VERTEX_SRC, FRAGMENT_SRC);
		m_program.addattribute("vertexPosition", POS);
		m_program.addattribute("vertexColor", COL);
		m_program.linkshaders();

		//set up vetex array buffer
		glGenVertexArrays(1,&m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		//set up bind attribute and attribute pointer
		glEnableVertexAttribArray(POS);
		glVertexAttribPointer(POS, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, position));
		glEnableVertexAttribArray(COL);
		glVertexAttribPointer(COL, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));

		glBindVertexArray(0);
	}

	glm::vec2 rotation(glm::vec2 originalvector,float angle) {
		glm::vec2 result;
		result.x = originalvector.x*cos(angle) - originalvector.y*sin(angle);
		result.y = originalvector.x*sin(angle) + originalvector.y*cos(angle);
		return result;
	}

	void DebugRender::drawBox(const glm::vec4& posrec, const ColorRGBA8& color, float angle) {
		int start = m_vertices.size();
		m_vertices.resize(start + 4);
		glm::vec2 pos(posrec.x, posrec.y);
		glm::vec2 halfdim(posrec.z / 2.0f, posrec.w / 2.0f);

		glm::vec2 lb(-halfdim.x, -halfdim.y);
		glm::vec2 rb(halfdim.x, -halfdim.y);
		glm::vec2 rt(halfdim.x, halfdim.y);
		glm::vec2 lt(-halfdim.x, halfdim.y);

		angle = angle / 180.0*PI;

		m_vertices[start].position = rotation(lb, angle) + pos;
		m_vertices[start + 1].position = rotation(rb, angle) + pos;
		m_vertices[start + 2].position = rotation(rt, angle) + pos;
		m_vertices[start + 3].position = rotation(lt, angle) + pos;
		for (int i = start;i < start + 4;i++) {
			m_vertices[i].color = color;
		}

		int istart = m_indexes.size();
		m_indexes.resize(istart + 8);

		m_indexes[istart] = start;
		m_indexes[istart + 1] = start + 1;

		m_indexes[istart + 2] = start + 1;
		m_indexes[istart + 3] = start + 2;

		m_indexes[istart + 4] = start + 2;
		m_indexes[istart + 5] = start + 3;

		m_indexes[istart + 6] = start + 3;
		m_indexes[istart + 7] = start;
	}

	void DebugRender::drawCircle(const glm::vec2 &center, float radius, const ColorRGBA8 &color) {
		static int NUM_VERTICES = 100;
		int start = m_vertices.size();
		m_vertices.resize(start + NUM_VERTICES);
		glm::vec2 temp;
		for (int j = 0;j < NUM_VERTICES;j++) {
			m_vertices[start + j].position.x = center.x + radius*cos(2 * PI*j / (float)NUM_VERTICES);
			m_vertices[start + j].position.y = center.y + radius*sin(2 * PI*j / (float)NUM_VERTICES);
			m_vertices[start + j].color = color;
		}

		int istart = m_indexes.size();
		int i = istart;
		m_indexes.resize(istart + NUM_VERTICES*2);
		for (int j = 0;j < NUM_VERTICES-1;j++) {
			//01,12,23,...,9899
			m_indexes[i++] = start + j;
			m_indexes[i++] = start + j + 1;
		}
		//99,0; i= istart+197
		m_indexes[i++] = start + NUM_VERTICES-1;
		m_indexes[i++] = start;
	
	}

	void DebugRender::drawLine(const glm::vec2& a, const glm::vec2& b, const ColorRGBA8 &color) {
		if (a != b) {
			int start = m_vertices.size();
			m_vertices.resize(start + 2);
			m_vertices[start].position = a;
			m_vertices[start].color = color;
			m_vertices[start + 1].position = b;
			m_vertices[start + 1].color = color;
			m_indexes.push_back(GLuint(start));
			m_indexes.push_back(GLuint(start + 1));
		}else {
			fatalError("The line has no Length!");
		}
	}

	void DebugRender::drawCapsule(const glm::vec4& desRec, const ColorRGBA8& color, float angle /*= 0.0f*/) {
		//draw box
		glm::vec4 boxDesRec(desRec - glm::vec4(0.0f, 0.0f, 0.0f, desRec.z));
		drawBox(boxDesRec, color,angle);

		//draw upper and lower circle
		glm::vec2 temp(0.0f, (desRec.w - desRec.z) / 2.0f);
		glm::vec2 circlepos(rotation(temp,angle)+glm::vec2(desRec.x,desRec.y));
		drawCircle(circlepos, desRec.z/2.0f, color);
		temp.y = -temp.y;
		glm::vec2 circlepos2(rotation(temp,angle)+ glm::vec2(desRec.x, desRec.y));
		drawCircle(circlepos2, desRec.z / 2.0f, color);
	}

	void DebugRender::drawGrid(const glm::vec4& desRec, const glm::vec2& tileDim, const ColorRGBA8& color/* = ColorRGBA8(255, 255, 255, 255)*/) {
		drawBox(desRec, color, 0.0f);
		int vertical_num = ceil(desRec.z / tileDim.x);
		int horizontal_num = ceil(desRec.w / tileDim.y);
		float x_origin = desRec.x - desRec.z / 2.0f;
		float x_max = desRec.x + desRec.z / 2.0f;
		int y_origin = desRec.y - desRec.w / 2.0f;
		float y_max = desRec.y + desRec.w / 2.0f;

		for (int x = 0;x < vertical_num;x++) {
			float xPos = x*tileDim.x + x_origin;
			drawLine(glm::vec2(xPos,y_origin), glm::vec2(xPos, y_max), color);
		}
		for (int y = 0;y < horizontal_num;y++) {
			float yPos = y*tileDim.y + y_origin;
			drawLine(glm::vec2(x_origin, yPos), glm::vec2(x_max, yPos), color);
		}
	}


	void DebugRender::end() {
		glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
		//orphan the data
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
		//upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(DebugVertex), m_vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		//orphan the data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexes.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
		//upload the data
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexes.size() * sizeof(GLuint), m_indexes.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//keep track of vertices number
		//use in draw call
		m_numVertices = m_indexes.size();
		//clean up every frame after upload the data
		m_vertices.clear();
		m_indexes.clear();
		
	}

	void DebugRender::render(glm::mat4 projectionmatrix,float linewidth) {
		m_program.use();
		glBindVertexArray(m_vao);

		GLint Ploc = m_program.getuniformposition("P");
		glUniformMatrix4fv(Ploc, 1, GL_FALSE, &projectionmatrix[0][0]);

		glLineWidth(linewidth);
		glDrawElements(GL_LINES,m_numVertices,GL_UNSIGNED_INT,0);

		glBindVertexArray(0);
		m_program.unuse();
	}

	void DebugRender::destroy() {
		if (m_vao) {
			glDeleteVertexArrays(1,&m_vao);
		}
		if (m_vbo) {
			glDeleteBuffers(1, &m_vbo);
		}
		if (m_ibo) {
			glDeleteBuffers(1, &m_ibo);
		}
		m_program.dispose();
	}

}