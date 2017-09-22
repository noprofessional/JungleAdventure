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
		bool clamped;
	};
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();
		GLtexture* addTexture(const std::string& fakeTexturePath,const int& width,const int& height,const std::vector<unsigned char>& data);
		GLtexture* gettexture(const std::string& texturepath);
		GLtexture* getSTClampedTexture(const std::string& texturePath);
	private:
		std::map<std::string, GLtexture> m_texturemap;
		std::map<std::string, GLtexture> m_clampedTextures;
	};
	static TextureCache textureCache;/// The game shares a same cache !!
}