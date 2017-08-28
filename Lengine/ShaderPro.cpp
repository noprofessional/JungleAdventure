#include "ShaderPro.h"
#include "FatalError.h"
#include "FileIO.h"
#include <vector>
#include <fstream>
namespace Lengine {

ShaderPro::ShaderPro() :_programID(0), _vertexshaderID(0), _fragmentshaderID(0)
{
}

ShaderPro::~ShaderPro()
{
}

//get the source from file
void ShaderPro::compileshaders(const std::string &vertextshaderfilepath, const std::string &fragmentshaderfilepath)
{
	//Get a program object.
	_programID = glCreateProgram();


	//Get shader haddle for vertex shader and fragment shader, then check for error
	_vertexshaderID = glCreateShader(GL_VERTEX_SHADER);
	if (_vertexshaderID == 0) {
		fatalError("Vertex shader failed to be created!");
	}

	_fragmentshaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragmentshaderID == 0) {
		fatalError("fragment shader failed to be created!");
	}

	//read file into string buffer
	std::string vertexshadersrc, fragmentshadersrc;
	FileIO::fileIntoBuffer(vertextshaderfilepath, vertexshadersrc);
	FileIO::fileIntoBuffer(fragmentshaderfilepath, fragmentshadersrc);


	//compile the shader program one at a time
	compileshader(vertexshadersrc.c_str(), vertextshaderfilepath,_vertexshaderID);
	compileshader(fragmentshadersrc.c_str(),fragmentshaderfilepath, _fragmentshaderID);
}

//get the source from inline cpp code file
void ShaderPro::compileShadersFromSource(const char * vertexsrc, const char* fragmentsrc) {
	//Get a program object.
	_programID = glCreateProgram();


	//Get shader haddle for vertex shader and fragment shader, then check for error
	_vertexshaderID = glCreateShader(GL_VERTEX_SHADER);
	if (_vertexshaderID == 0) {
		fatalError("Vertex shader failed to be created!");
	}

	_fragmentshaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (_fragmentshaderID == 0) {
		fatalError("fragment shader failed to be created!");
	}

	compileshader(vertexsrc, "inline vertex shader", _vertexshaderID);
	compileshader(fragmentsrc, "inline fragment shader", _fragmentshaderID);
}

void ShaderPro::linkshaders()
{
	//Vertex and fragment shaders are successfully compiled.
	//Now time to link them together into a program.



	//Attach our shaders to our program
	glAttachShader(_programID, _vertexshaderID);
	glAttachShader(_programID, _fragmentshaderID);

	//Link our program
	glLinkProgram(_programID);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

		//We don't need the program anymore.
		glDeleteProgram(_programID);
		//Don't leak shaders either.
		glDeleteShader(_vertexshaderID);
		glDeleteShader(_fragmentshaderID);

		std::printf("%s/n", &(errorLog[0]));
		fatalError("Shader failed to link!");

	}

	//Always detach shaders after a successful link.
	glDetachShader(_programID, _vertexshaderID);
	glDetachShader(_programID, _fragmentshaderID);
	glDeleteShader(_vertexshaderID);
	glDeleteShader(_fragmentshaderID);

}

//bind attribute with index(used in data upload rearange)
void ShaderPro::addattribute(const std::string & attributname,Index attribeindex) {
	glBindAttribLocation(_programID, attribeindex, attributname.c_str());
}

void ShaderPro::use() {
	glUseProgram(_programID);
}

void ShaderPro::unuse() {
	glUseProgram(0);
}

void ShaderPro::dispose() {
	if (_vertexshaderID)glDeleteShader(_vertexshaderID);
	if (_fragmentshaderID)glDeleteShader(_fragmentshaderID);
	if (_programID)glDeleteProgram(_programID);
}


GLint ShaderPro::getuniformposition(std::string uniformname) {
	GLint pos = glGetUniformLocation(_programID, uniformname.c_str());
	if (pos == GL_INVALID_INDEX)
		fatalError("Uniform " + uniformname + " cannot be found");
	return pos;
}

void ShaderPro::compileshader(const char* shadersrc,const std::string& name, GLuint ID)
{

	//find source and compile
	const GLchar *contentsptr = (GLchar *)shadersrc;
	glShaderSource(ID, 1, &contentsptr, nullptr);
	glCompileShader(ID);

	//compile error checking
	GLint success = 0;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);


	if (success == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(ID, maxLength, &maxLength, &(errorLog[0]));

		std::printf("%s\n", &(errorLog[0]));

		fatalError("Shader " + name + " failed to compile!");

	}
}

}