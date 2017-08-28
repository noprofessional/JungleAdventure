#include "Window.h"
#include "FatalError.h"

namespace Lengine {
Window::Window()
{
	_window = nullptr;
	_screenheight = 0;
	_screenwidth = 0;
}


Window::~Window()
{
}

//init all sdl component, 
//create a window with blue background,
//flag: INVISIBLE, FULLSCREEN, BOARDERLESS. defult to 0
bool Window::create(std::string windowname, int screenwidth, int screenheight, unsigned int setflag) {


	//Create buffer and swap to avoid fleeker;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (setflag&INVISIBLE) {
		flags |= SDL_WINDOW_HIDDEN;
	}
	if (setflag&FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (setflag&BOARDLESS) {
		flags |= SDL_WINDOW_BORDERLESS;
	}

	_screenwidth = screenwidth;
	_screenheight = screenheight;

	_window = SDL_CreateWindow(windowname.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenwidth, screenheight, flags);
		
	//check if window generate correctly
	if (_window == nullptr)
		fatalError("SDL Window cannot be created!");

	//Sotre all context of the window created and check error
	SDL_GLContext glcontext = SDL_GL_CreateContext(_window);
	if (glcontext == nullptr)
		fatalError("SDL Context cannot be created!");

	//initialize glew
	GLenum Err = glewInit();
	if (Err != GLEW_OK) {
		//print out the error log
		fatalError("Glew init error : %s" + std::string((char*)glewGetErrorString(Err)));
	}

	//Set the color buffer to blue
	//and transparent to the source
	glClearColor(0.0f,0.0f,1.0f,1.0f);
	//depth clear to 1.0f
	glClearDepth(1.0f);

	//syc the frame rate with screen 
	SDL_GL_GetSwapInterval();

	glEnable(GL_BLEND);
	//source color multiple, background color multiple
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	return 0;

	}

//change background color
//r,g,b,a should be in 0 to 1
void Window::setcolor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

//clean the window every frame
void Window::clean() {
	//clear to preset color and depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//swap buffer
void Window::swapbuffer() {
	SDL_GL_SwapWindow(_window);
}
}