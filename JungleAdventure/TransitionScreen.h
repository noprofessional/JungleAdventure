#pragma once
#include <Lengine/Iscreen.h>
class TransitionScreen :public Lengine::IScreen
{
public:
	TransitionScreen(Lengine::IMainGame* ownerGame);
	~TransitionScreen();
	void build() override;
	void destroy() override;
	void update() override;
	void draw() override;
private:

};

