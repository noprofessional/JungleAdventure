#pragma once
#include<vector>
#include<SDL/SDL.h>
#include<glm/glm.hpp>
#include"Vertex.h"
#include"ShaderPro.h"
namespace Lengine {
	class DebugRender
	{
	public:
		DebugRender();
		~DebugRender();
		void init();

		void drawBox(const glm::vec4& posrec, const ColorRGBA8& color, float angle);
		void drawCircle(const glm::vec2 &center, float radius, const ColorRGBA8 &color);
		void drawLine(const glm::vec2& a, const glm::vec2& b, const ColorRGBA8 &color);
		void drawCapsule(const glm::vec4& desRec, const ColorRGBA8& color, float angle=0.0f);

		void end();
		void render(glm::mat4 projectionmatrix,float linewidth);
		void destroy();
		struct DebugVertex {
			glm::vec2 position;
			ColorRGBA8 color;
		};
		int getnum() { return m_numVertices; }
	private:
		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_ibo;
		std::vector<DebugVertex> m_vertices;
		int m_numVertices=0;
		std::vector<GLuint> m_indexes;
		ShaderPro m_program;
	};

}