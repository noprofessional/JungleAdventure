#pragma once
#include <vector>

namespace Lengine {
	typedef std::vector<unsigned char> uCharVec;
	typedef std::vector<uCharVec> ucharSheet;//contain multiple char array
	extern void loadGif(const char* filePath, ucharSheet& out, int& width, int&height);
}