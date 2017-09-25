#include "GameScreen.h"
#include "ScreenIdentifier.h"
#include <Lengine/TextureCache.h>
#include "LevelWriterNReader.h"
#include "ContactListener.h"
GameScreen::GameScreen(Lengine::IMainGame* ownergame):IScreen(ownergame,GAME_SCREEN)
{
}

GameScreen::~GameScreen()
{
}

void GameScreen::build() {

	//window related init
	m_background = BackGround(Lengine::ColorRGBA8(100, 100, 255, 255), 7);
	m_background.setAsCurrent(m_game->getWindowPtr());
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

	m_fontBatch.init("Fonts/eng.ttf", 32);

	m_debugRender.init();
	m_debuging = true;

	m_camera.init(WINDOW_WIDTH,WINDOW_HEIGHT);
	m_camera.setposition(glm::vec2(0.0f,0.0f));
	m_camera.setscale(32.0f);	//means 1 meter is 32 pixel on screen
	m_camera.change();

	//set the gravity of the world
	b2Vec2 Gravity(0.0f, -10.0f);
	m_world = std::make_unique<b2World>(Gravity);

	static ContactListener contactListener;
	m_world->SetContactListener(&contactListener);

	//load the level add to the world
	LevelWriterNReader::readAsBinary("Levels/test.txt.level", m_player, m_boxes, m_tiles, m_lights);

	m_player.addToWorld(m_world.get());
	m_boxes.addToWorld(m_world.get());

	initUI();
}

void GameScreen::initUI() {
	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	b_back = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.95f, 0.05f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 80.0f, 30.0f), "Root/Button"));
	b_back->setText("Back");
	b_back->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameScreen::onBackClicked, this));
}

void GameScreen::destroy() {
	if (m_world.get())
	{
		b2World* wptr = m_world.release();
		delete wptr;
	}
	m_gui.destroy();
	m_lightPro.dispose();
	m_program.dispose();
}

void GameScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		basicInputProcess(evnt);
		m_gui.injectEvent(evnt);
	}
}

void GameScreen::update() {
	m_background.update();

	m_player.update(m_game->getInputManager());

	cameraTrack();
	m_camera.change();

	m_world->Step(1.0f / 60.0f, 8, 3);
}

void GameScreen::draw() {

	m_background.draw(&m_program, &m_spriteBatch);

	glm::mat4 Projection = m_camera.getcameramatrix();

	//boxes and player render
	{
		m_program.use();

		glActiveTexture(GL_TEXTURE0);
		GLint samplerLoc = m_program.getuniformposition("mysampler");
		glUniform1i(samplerLoc, 0);

		GLint pLoc = m_program.getuniformposition("P");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection[0][0]);

		m_spriteBatch.begin(Lengine::GlyphSortType::BACK_TO_FRONT);

		m_player.draw(&m_spriteBatch);

		m_tiles.drawTexture(&m_spriteBatch, &m_camera);

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

		m_lights.draw(&m_lightBatch);

		m_lightBatch.end();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		m_lightBatch.renderBatch();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_lightPro.unuse();

	}
	//font render
	{
		Lengine::Camera tempCam;
		tempCam = m_camera;
		glm::ivec2 screenDim = tempCam.getScreenDim();
		tempCam.setposition(glm::vec2(0.0f));
		tempCam.setscale(1.0f);
		tempCam.change();

		static int frame = 0;
		frame++;
		static std::string fps;
		std::string tempstr = std::to_string(m_game->getFpsLimiter()->calculateFPS());
		if (frame > 10) {
			fps = tempstr;
			frame = 0;
		}

		m_program.use();

		glActiveTexture(GL_TEXTURE0);
		GLint samplerLoc = m_program.getuniformposition("mysampler");
		glUniform1i(samplerLoc, 0);

		glm::mat4 Projection1 = tempCam.getcameramatrix();
		GLint pLoc = m_program.getuniformposition("P");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection1[0][0]);

		m_spriteBatch.begin(Lengine::GlyphSortType::BACK_TO_FRONT);

		m_fontBatch.draw(m_spriteBatch, fps.c_str(), glm::vec2(screenDim.x / 2.0f - 19.0f, screenDim.y / 2.0f - 10.0f), glm::vec2(1.0f), 10.0f, Lengine::RED);

		m_spriteBatch.end();

		m_spriteBatch.renderBatch();

		m_program.unuse();

	}
	//debug render
	if (m_debuging) {

		m_player.debugDraw(&m_debugRender);
		m_boxes.debugDraw(&m_debugRender);
		m_lights.debugDraw(&m_debugRender, m_debuging);

		m_debugRender.end();

		m_debugRender.render(Projection, 0.5f);
	}

	m_gui.draw();
}

void GameScreen::cameraTrack(){

	glm::vec2 playerMove = m_player.getMove();
	glm::vec2 playerPos = m_player.getPos();

	if (playerPos.x>-3.0f)
	{
		m_camera.offsetPosition(glm::vec2(playerMove.x,0.0f));
	}
	if (playerPos.y>-3.0f) {
		m_camera.offsetPosition(glm::vec2(0.0f, playerMove.y));
	}

}

bool GameScreen::onBackClicked(const CEGUI::EventArgs& eas) {
	m_nextScreenIndex = MENU_SCREEN;
	m_state = Lengine::ScreenState::TO_NEXT;
	return true;
}
