#pragma once
#include <Lengine/IMainGame.h>

class App:public Lengine::IMainGame
{
public:
	App();
	~App();
	void init() override;
};

