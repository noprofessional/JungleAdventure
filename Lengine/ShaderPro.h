#pragma once

#include <string>
#include <GL/glew.h>
#include "Vertex.h"
namespace Lengine {

	class ShaderPro
	{
	public:
		ShaderPro();
		~ShaderPro();

		void compileshaders(const std::string &vertextshaderfilepath, const std::string &fragmentshaderfilepath);
		void compileShadersFromSource(const char * vertexsrc,const char* fragmentsrc);
		void linkshaders();
		void addattribute(const std::string & attributname,Index attribeindex);

		void use();
		void unuse();
		void dispose();
		GLint getuniformposition(std::string uniformname);

	private:
		void compileshader(const char* vertexsrc,const std::string& shadername, GLuint ID);
		GLuint _programID;
		GLuint _vertexshaderID;
		GLuint _fragmentshaderID;
	};

}