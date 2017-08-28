#include "ResourceManager.h"
namespace Lengine {
	TextureCache ResourceManager::texturecache;

	GLtexture ResourceManager::gettexture(std::string texturepath) {
		return texturecache.gettexture(texturepath);
	}
}