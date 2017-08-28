#pragma once
#include "TextureCache.h"
#include <string>
namespace Lengine {
	class ResourceManager
	{
	public:
		static GLtexture gettexture(std::string texturepath);
	private:
		static TextureCache texturecache;
	};

}