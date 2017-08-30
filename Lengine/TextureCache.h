#pragma once
#include <map>
#include <string>
#include <GL/glew.h>
#include<vector>

namespace Lengine {
	struct GLtexture {
		std::string filePath;
		std::vector<GLuint> ids;
		int height;
		int width;
	};
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();
		GLtexture gettexture(const std::string& texturepath);
	private:
		std::map<std::string, GLtexture> m_texturemap;
	};

}