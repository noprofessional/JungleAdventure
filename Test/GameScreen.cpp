#include "GameScreen.h"
#include "ScreenIdentifier.h"
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

	m_camera.init(WINDOW_WIDTH,WINDOW_HEIGHT);
	m_camera.setposition(glm::vec2(0.0f,0.0f));
	m_camera.setscale(32.0f);

	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.setFont("DejaVuSans-10");
	m_pushButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.9f, 0.1f, 0.0f, 0.0f), glm::vec4(0.15f, 0.05f, 0.0f, 0.0f), "BackButton"));
	m_pushButton->setText("Back to Menu");
	m_pushButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameScreen::actionExit, this));
	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);

	b2Vec2 Gravity(0.0f, -9.8f);
	m_world =std::make_unique<b2World>(Gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.type = b2_staticBody;
	groundBodyDef.position.Set(0.0f, -20.0f);
	b2Body* ground = m_world->CreateBody(&groundBodyDef);
	b2PolygonShape groundShape;
	groundShape.SetAsBox(20.0f, 10.0f);
	ground->CreateFixture(&groundShape, 1.0f);

	LevelWriterNReader::readAsText(std::string("Levels/level1.txt"), m_player, m_boxes, m_lights);

	static Lengine::GLtexture testGif = m_game->getTextureCache()->gettexture("Textures/spr_m_traveler_idle_anim.gif");
	m_player.tempSetAll(glm::vec4(0.0f, 0.0f, 2.0f, 2.0f), testGif);
	m_player.addToWorld(m_world.get(), glm::vec2(0.9f, 1.8f), glm::vec2(0.0f, -0.1f));

	m_lights.push_back(Light{});
	m_lights.back().centerPos = glm::vec2(10.0f, 10.0f);
	m_lights.back().color = Lengine::ColorRGBA8(255, 255, 255, 255);
	m_lights.back().size = 10.0f;

	static Lengine::GLtexture boxTex = m_game->getTextureCache()->gettexture("Textures/darkwall.png");
	m_boxes.push_back(Box());
	m_boxes.back().tempSetAll(glm::vec4(-5.0f, -7.0f, 2.0f, 2.0f), 0.0f, Lengine::ColorRGBA8(255, 255, 255, 255), boxTex, PhysicMode::RIGID);
	m_boxes.push_back(Box());
	m_boxes.back().tempSetAll(glm::vec4(-8.0f, 5.0f, 2.0f, 2.0f), 0.0f, Lengine::ColorRGBA8(255, 255, 255, 255), boxTex, PhysicMode::DYNAMIC);
	for (auto& B : m_boxes) {
		B.addToWorld(m_world.get());
	}

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
	m_camera.change();
	m_world->Step(1/60.0f, 8, 2);
	m_game->getInputManager()->update();
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

	m_player.draw(&m_spriteBatch);
	for (auto& B : m_boxes) {
		B.draw(&m_spriteBatch);
	}
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_program.unuse();

	m_player.debugDraw(&m_debugRender);
	m_debugRender.end();
	m_debugRender.render(Projection,1.0f);

	m_lightPro.use();
	m_lightBatch.begin();
	GLint lightProPLoc = m_lightPro.getuniformposition("P");
	glUniformMatrix4fv(lightProPLoc, 1, GL_FALSE, &Projection[0][0]);
	for (auto L : m_lights) {
		L.draw(&m_lightBatch);
	}
	m_lightBatch.end();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_lightBatch.renderBatch();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_lightPro.unuse();

	m_gui.draw();
}

bool GameScreen::actionExit(const CEGUI::EventArgs& args) {
	m_game->setGameState(Lengine::GameState::EXIT);
	return true;
}
