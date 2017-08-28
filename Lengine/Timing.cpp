#include "Timing.h"
#include<iostream>
namespace Lengine {

//actual delay process
//need begin() at the begining of the loop
void FpsLimiter::limit() {
	int frameTicks = SDL_GetTicks() - m_limitTickCounter;
	//std::cout << frameTicks<<std::endl;
	float minFrametime = MILLISECOND_PER_SECOND / m_maxFPS;
	//when use less than minest frame time delay
	if (minFrametime > float(frameTicks)) {
		SDL_Delay((Uint32)(minFrametime - frameTicks));
	}
	m_limitTickCounter = SDL_GetTicks();
}

//calculate FPS and return the FPS
//need begin() at the begining of the loop
float FpsLimiter::calculateFPS() {
	int frametime = SDL_GetTicks() - m_calculateTickCounter;
	
	//save each frame time into an circled array
	frametimes[(m_framecount++) % 10] = frametime;

	float fps = 60.0f;
	float averageTicksPerFrame = 0.0f;

	if (m_framecount >10) {
		//add together only if have more than 10 num recorded
		for (int i = 0;i < 10;i++)
		{
			averageTicksPerFrame += frametimes[i];
		}
		averageTicksPerFrame = averageTicksPerFrame /10;
		fps = MILLISECOND_PER_SECOND / averageTicksPerFrame;
	}

	//else return 60.0f fps
	m_calculateTickCounter = SDL_GetTicks();
	return fps;
}

//calculat delta time and return
float FpsLimiter::deltatime()
{
	int TicksPerFrame = SDL_GetTicks() - m_deltaTickCounter;
	float ticksTarget = MILLISECOND_PER_SECOND / m_targetFPS;

	//actual_ticks_per_render/supposed_ticks_per_frame =actual_frame_per_render
	//should calculate deltatime frames before render
	float deltatime;

	deltatime = float(TicksPerFrame) / ticksTarget;
	
	m_deltaTickCounter = SDL_GetTicks();
	return deltatime;
}

}