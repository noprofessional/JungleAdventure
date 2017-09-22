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
	destroy();
}

void GameScreen::build() {
	capture = true;
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
	int width;
	int height;
	std::vector<unsigned char> frameData;
	LevelWriterNReader::readAsBinary("Levels/level1.txt.level", m_player, m_boxes, m_lights, frameData, width, height);
	frameTexture = Lengine::textureCache.addTexture("Levels/level1.txt.level", width, height, frameData);

	m_player.addToWorld(m_world.get());
	for (auto & B : m_boxes) {
		B.addToWorld(m_world.get());
	}

	SDL_ShowCursor(1);

}

void GameScreen::destroy() {
}

void GameScreen::process() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		basicInputProcess(evnt);
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
		/*for (auto& B : m_boxes) {
			B.clampedDraw(&m_spriteBatch);
		}*/
		int SW = m_game->getWindowPtr()->getscreenwidth();
		int SH = m_game->getWindowPtr()->getscreenheight();
		float x = (-SW + frameTexture->width) / 2.0f / m_camera.getscale();
		float y = (-SH + frameTexture->height) / 2.0f / m_camera.getscale();
		m_spriteBatch.draw(glm::vec4(x,y, frameTexture->width/32.0f, frameTexture->height/32.0f), glm::vec4(0.0f, 1.0f, 1.0f, -1.0f), frameTexture->ids[0], 1.0f);
		m_spriteBatch.end();
		
		m_spriteBatch.renderBatch();
		
		m_program.unuse();
	}
	//light render
	{

		m_lightBatch.begin();
		for (auto & L : m_lights) {
			L.draw(&m_lightBatch);
		}
		m_lightBatch.end();

		m_lightPro.use();
		GLint pLoc = m_lightPro.getuniformposition("P");
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, &Projection[0][0]);

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

	if (capture) {
		buffer.resize(100 * 100 * 24);
		glReadPixels(950, 650, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

		capture = false;
	}
}

void GameScreen::cameraTrack() {
	const float RANGE = 1.0f;
	static glm::vec2 prePlayerPos = m_player.getCapsule().getpos();
	glm::vec2 playerPos = m_player.getCapsule().getpos();
	glm::vec2 playerMove = m_player.getCapsule().getpos()-prePlayerPos;
	prePlayerPos = playerPos;
	if (playerPos.x>-3.0f)
	{
		m_camera.offsetPosition(glm::vec2(playerMove.x,0.0f));
	}
	if (playerPos.y>-3.0f) {
		m_camera.offsetPosition(glm::vec2(0.0f, playerMove.y));
	}

}