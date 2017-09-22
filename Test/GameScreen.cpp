#include "GameScreen.h"
#include "ScreenIdentifier.h"
#include "Box.h"
GameScreen::GameScreen(Lengine::IMainGame* ownergame):IScreen(ownergame,GAME_SCREEN)
{
}

GameScreen::~GameScreen()
{
	destroy();
}

void GameScreen::build() {
	//window related init
	m_game->getWindowPtr()->setcolor(0.7f, 0.7f, 1.0f, 1.0f);
	const float WINDOW_WIDTH = m_game->getWindowPtr()->getscreenwidth();
	const float WINDOW_HEIGHT = m_game->getWindowPtr()->getscreenheight();

	m_program.compileshaders(std::string("Shaders/shaderpro.vert"), std::string("Shaders/shaderpro.frag"));
	m_program.addattribute("vertexPosition", Lengine::POS);
	m_program.addattribute("vertexColor", Lengine::COL);
	m_program.addattribute("vertexuv", Lengine::UVC);
	m_program.linkshaders();

	m_spriteBatch.init();
	m_secondBatch.init();

	m_camera.init(WINDOW_WIDTH,WINDOW_HEIGHT);
	m_camera.setposition(glm::vec2(0.0f,0.0f));
	m_camera.setscale(32.0f);

	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	m_pushButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.9f, 0.1f, 0.0f, 0.0f), glm::vec4(0.15f, 0.05f, 0.0f, 0.0f), "BackButton"));
	m_pushButton->setText("EXIT");
	m_pushButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameScreen::actionExit, this));
	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);

	m_debugRender.init();

}
void GameScreen::destroy() {
}

void GameScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_gui.injectEvent(evnt);
		basicInputProcess(evnt);
	}
}
void GameScreen::update() {
	Lengine::InputManager* im = m_game->getInputManager();
	if (im->isKEYdown(SDLK_UP)) {
		m_camera.offsetPosition(glm::vec2(0.0f, 0.2f));
	}
	else if(im->isKEYdown(SDLK_DOWN)) {
		m_camera.offsetPosition(glm::vec2(0.0f, -0.2f));
	}
	if (im->isKEYdown(SDLK_z)) {
		m_camera.offsetScale(0.5f);
	}
	if (im->isKEYpressed(SDLK_r)) {
		m_camera.setposition(glm::vec2(0.0f, 0.0f));
		m_camera.setscale(32.0f);
	}
	m_camera.change();
}
void GameScreen::draw() {
	m_program.use();

	glActiveTexture(GL_TEXTURE0);
	GLint samplerLoc = m_program.getuniformposition("mysampler");
	glUniform1i(samplerLoc, 0);

	GLint pLoc = m_program.getuniformposition("P");
	glm::mat4 Projection = m_camera.getcameramatrix();
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection[0][0]);

	m_spriteBatch.begin();
	glm::vec4 pos(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 uv(0.5f, 0.5f, 0.5f, 0.5f);
	static Lengine::GLtexture* test = Lengine::textureCache.getSTClampedTexture("Textures/b_r_g_b.png");
	m_spriteBatch.draw(pos, uv, test->ids[0], 1.0f);

	m_spriteBatch.end();

	m_spriteBatch.renderBatch();

	//m_spriteBatch.begin();
	//m_spriteBatch.draw(glm::vec4(0.0f, -32.0f, 320.0f, 32.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), test2->ids[0], 1.0f);
	//m_spriteBatch.end();
	//float scale = m_camera.getscale();
	//m_camera.setscale(scale/32.0f);
	//m_camera.change();
	//glm::mat4 Projection1 = m_camera.getcameramatrix();
	//glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection1[0][0]);
	//m_spriteBatch.renderBatch();
	//m_camera.setscale(scale);
	//m_camera.change();

	m_program.unuse();
	m_gui.draw();
}

bool GameScreen::actionExit(const CEGUI::EventArgs& args) {
	m_game->setGameState(Lengine::GameState::EXIT);
	return true;
}
