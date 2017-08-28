#pragma once
#include<SDL\SDL.h>
namespace Lengine {

const int MILLISECOND_PER_SECOND = 1000;

class FpsLimiter {
public:

	//----settet----
	void setMaxFPS(float maxFPS) { m_maxFPS = maxFPS; }
	void setTargetFPS(float targetfps) { m_targetFPS = targetfps; }
	//----getter----
	float getMaxFPS() { return m_maxFPS; }
	float getTargetFPS() { return m_targetFPS; }

	void begin();
	void limit();
	float calculateFPS();
	float deltatime();
private:
	float m_maxFPS=60.0f;
	float m_targetFPS=60.0f;
	//----Tick Counters
	int m_calculateTickCounter=0;
	int m_limitTickCounter=0;
	int m_deltaTickCounter=0;

	int m_framecount=0;
	int frametimes[10];
};
}
