#include "GameEngine.h"
#include<SDL/SDL.h>
#include<GL/glew.h>
#include<string>
#include"FatalError.h"
namespace Lengine {
	bool GameEngine::engineInit = false;

	void GameEngine::init() {
		if (engineInit == false) {
			//initialize sdl
			int errorcode = SDL_Init(SDL_INIT_EVERYTHING);
			if (errorcode != 0) {
				fatalError("SDL initialize failed : " + std::string(SDL_GetError()));
			}
			engineInit = true;
		}
		else {
			fatalError("Don't initialize GameEngine MORE THAN ONCE!");
		}
	}
}