#include "GameScreen.h"
#include "ScreenIdentifier.h"
#include<Lengine/ResourceManager.h>
#include "LevelWriterNReader.h"
GameScreen::GameScreen(Lengine::IMainGame* ownergame):IScreen(ownergame,GAME_SCREEN)
{
}

GameScreen::~GameScreen()
{
	destroy();
}

void GameScreen::build() {
	//window related init
	m_game->getWindowPtr()->setcolor(0.0f, 0.0f, 0.0f, 1.0f);
	const float WINDOW_WIDTH = m_game->getWindowPtr()->getscreenwidth();
	const float WINDOW_HEIGHT = m_game->getWindowPtr()->getscreenheight();

	m_program.compileshaders(std::string("Shaders/shaderpro.vert"), std::string("Shaders/shaderpro.frag"));
	m_program.addattribute("vertexPosition", Lengine::POS);
	m_program.addattribute("vertexColor", Lengine::COL);
	m_program.addattribute("vertexuv", Lengine::UVC);
	m_program.linkshaders();

	m_lightPro.compileshaders(std::string("Shaders/lightshader.vert"), std::string("Shaders/lightshader.frag"));
	m_lightPro.addattribute("vertexPosition", Lengine::POS);
	m_lightPro.addattribute("vertexColor", Lengine::COL);
	m_lightPro.addattribute("vertexuv", Lengine::UVC);
	m_lightPro.linkshaders();

	m_spriteBatch.init();
	m_lightBatch.init();

	m_debugRender.init();
	m_debuging = true;

	m_camera.init(WINDOW_WIDTH,WINDOW_HEIGHT);
	m_camera.setposition(glm::vec2(0.0f,0.0f));
	m_camera.setscale(32.0f);	//means 1 meter is 32 pixel on screen

	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.loadScheme("AlfiskoSkin.scheme");
	m_gui.setFont("DejaVuSans-10");
	CEGUI::PushButton* BackButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("AlfiskoSkin/Button", glm::vec4(0.9f, 0.1f, 0.0f, 0.0f), glm::vec4(0.15f, 0.05f, 0.0f, 0.0f), "BackButton"));
	BackButton->setText("Back to Menu");
	BackButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameScreen::actionBack,this));
	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);

	//set the gravity of the world
	b2Vec2 Gravity(0.0f, -20.0f);
	m_world = std::make_unique<b2World>(Gravity);

	//load the level add to the world
	LevelWriterNReader::readAsText("Levels/level1.txt", m_player, m_boxes, m_lights);
	m_player.addToWorld(m_world.get());
	for (auto & B : m_boxes) {
		B.addToWorld(m_world.get());
	}

}
void GameScreen::destroy() {
	m_gui.destroy();
}

void GameScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_gui.injectEvent(evnt);
		basicInputProcess(evnt);
	}
}
void GameScreen::update() {
	m_world->Step(1.0f / 60.0f, 8, 3);
	m_camera.change();
	m_player.update(m_game->getInputManager());
	m_game->getInputManager()->update();
}
void GameScreen::draw() {

	glm::mat4 Projection = m_camera.getcameramatrix();

	//boxes and player render
	{
		m_program.use();

		glActiveTexture(GL_TEXTURE0);
		GLint samplerLoc = m_program.getuniformposition("mysampler");
		glUniform1i(samplerLoc, 0);


		GLint pLoc = m_program.getuniformposition("P");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection[0][0]);

		m_spriteBatch.begin();

		m_player.draw(&m_spriteBatch);

		for (auto& B : m_boxes) {
			B.draw(&m_spriteBatch);
		}
		
		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_program.unuse();
	}
	//light render
	{
		m_lightPro.use();
		GLint pLoc = m_lightPro.getuniformposition("P");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection[0][0]);
		m_lightBatch.begin();
		for (auto & L : m_lights) {
			L.draw(&m_lightBatch);
		}
		m_lightBatch.end();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		m_lightBatch.renderBatch();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_lightPro.unuse();

	}
	//debug render
	if (m_debuging) {
		m_player.debugDraw(&m_debugRender);
		for (auto&B : m_boxes) {
			B.debugDraw(&m_debugRender);
		}
		for (auto & L : m_lights) {
			L.debugDraw(&m_debugRender);
		}
		m_debugRender.end();
		m_debugRender.render(Projection, 0.5f);
	}

	m_gui.draw();

}

bool GameScreen::actionBack(const CEGUI::EventArgs& args) {
	m_state = Lengine::ScreenState::TO_PREVIOUS;
	m_previousScreenIndex = MENU_SCREEN;
	return true;
}
