#pragma once
#include <string>

#include <SDL\SDL.h>
#include <GL\glew.h>

#include "Vertex.h"
namespace Lengine {
enum  Windowflags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BOARDLESS = 0x4 };

class Window
{
public:
	Window();
	~Window();

	bool create(std::string windowname, int screenwidth, int screenheight, unsigned int setflag=0);
	void setcolor(float r, float g, float b, float a);
	void clean();
	void swapbuffer();

	int getscreenwidth() { return _screenwidth; }
	int getscreenheight() { return _screenheight; }

private:
	SDL_Window* _window;
	int _screenwidth, _screenheight;
};
}
