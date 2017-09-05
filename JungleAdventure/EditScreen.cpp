#include "EditScreen.h"
#include "ScreenIdentifier.h"
#include "LevelWriterNReader.h"
#include <Lengine/IMainGame.h>
#include <iostream>
#include <Lengine\FileIO.h>
#include <Lengine\TextureCache.h>
#include <Lengine/fatalError.h>

const float LIGHT_SELECT_RADIUS = 0.5f;
const Lengine::ColorRGBA8 WHITE(255, 255, 255, 255);
const glm::vec2 PLAYER_RENDER_DIM(2.0f, 2.0f);
const glm::vec2 PLAYER_POS_OFFSET(0.0f, -0.15f);
const glm::vec2 PLAYER_COLLISOIN_DIM(1.0f, 1.75f);


EditScreen::EditScreen(Lengine::IMainGame* ownergame):IScreen(ownergame,EDIT_SCREEN)
{
}

EditScreen::~EditScreen()
{
	destroy();
}

void EditScreen::build()
{
	m_game->getWindowPtr()->setcolor(0.5f, 0.5f, 0.5f, 1.0f);
	const int SCREEN_WIDTH = m_game->getWindowPtr()->getscreenwidth();
	const int SCREEN_HEIGHT = m_game->getWindowPtr()->getscreenheight();
	m_UIcamera.init(SCREEN_WIDTH,SCREEN_HEIGHT );
	m_UIcamera.setposition(glm::vec2(0.0f, 0.0f));
	m_UIcamera.setscale(1.0f);

	m_camera.init(SCREEN_WIDTH, SCREEN_HEIGHT);
	m_camera.setposition(glm::vec2(0.0f, 0.0f));
	m_camera.setscale(32.0f);

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
	m_lightSprite.init();

	m_spriteFont.init("Fonts/eng.ttf", 32);

	m_debugRenderer.init();

	b2Vec2 gravity(0.0f, -9.8f);
	m_world = std::make_unique<b2World>(gravity);

	m_texture = Lengine::textureCache.gettexture("Textures/white.png");
	m_playerTexture = Lengine::textureCache.gettexture("Textures/spr_m_traveler_idle_anim.gif");

	initUI();
}
void EditScreen::initUI() {
	m_gui.init("GUI");
	m_gui.loadScheme("TaharezLook.scheme");
	m_gui.loadScheme("AlfiskoSkin.scheme");
	m_gui.setFont("DejaVuSans-10");

	m_gui.setMouseCursor("TaharezLook/MouseArrow");
	m_gui.showMouseCursor();
	SDL_ShowCursor(0);

	//root window event
	m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&EditScreen::onMouseUp, this));
	m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&EditScreen::onMouseClicked, this));
	m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&EditScreen::onMouseDown, this));
	

	//group box init
	float groupWidth = 200.0f; float& GW = groupWidth;
	float groupHeight = 450.0f; float& GH = groupHeight;
	float Margin = 10.0f;
	CEGUI::Window* temp = m_gui.createWidget("TaharezLook/GroupBox",glm::vec4(0.0f, 1.0f, GW / 2.0f + Margin, -GH / 2.0f - Margin),glm::vec4(0.0f, 0.0f, GW, GH), "Group1");
	m_group = static_cast<CEGUI::GroupBox*>(temp);
	m_group->setAlwaysOnTop(false);
	m_group->moveToBack();
	m_group->subscribeEvent(CEGUI::GroupBox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&EditScreen::onWindowSelecte, this));
	m_group->subscribeEvent(CEGUI::GroupBox::EventMouseButtonUp, CEGUI::Event::Subscriber(&EditScreen::onMouseUp, this));

	float yPixelOPoint = 385.0f;
	//three slider init and bound to the group box
	{
		m_color = Lengine::ColorRGBA8(255, 255, 255, 255);
		float xPixelPos = 18.0f;
		float yPixelPos = yPixelOPoint;
		float deltaXPixel = 28.0f;
		glm::vec4 sliderSizeRec(0.0f, 0.0f, 15.0f, 55.0f);
		s_red = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), sliderSizeRec, "Grop1/RedSlider", temp));
		s_red->subscribeEvent(CEGUI::Slider::EventValueChanged,CEGUI::Event::Subscriber(&EditScreen::onRedSlideChange, this));
		s_red->setClickStep(1.0f);
		s_red->setMaxValue(255.0f);
		
		s_green = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos), sliderSizeRec, "Group1/GreenSlider", temp));
		s_green->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onGreenSlideChange, this));
		s_green->setClickStep(1.0f);
		s_green->setMaxValue(255.0f);

		s_blue = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(0.0f, 0.0f, xPixelPos + 2 * deltaXPixel, yPixelPos), sliderSizeRec, "Group1/BlueSlider", temp));
		s_blue->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onBlueSlideChange, this));
		s_blue->setClickStep(1.0f);
		s_blue->setMaxValue(255.0f);

		s_alpha = static_cast<CEGUI::Slider*>(m_gui.createWidget("TaharezLook/Slider", glm::vec4(0.0f, 0.0f, xPixelPos + 3 * deltaXPixel, yPixelPos), sliderSizeRec, "Group1/AlphaSlider", temp));
		s_alpha->subscribeEvent(CEGUI::Slider::EventValueChanged,CEGUI::Event::Subscriber(&EditScreen::onAlphaSliderChange, this));
		s_alpha->setClickStep(1.0f);
		s_alpha->setMaxValue(255.0f);
	}
	//select OR place AND debug
	{
		const int GROUP_ID_ZERO = 0;
		glm::vec4 buttonSizeRec(0.0f, 0.0f, 20.0f, 20.0f);

		float xPixelPos = 30.0f;
		float yPixelPos = yPixelOPoint - 55.0f;
		float deltaXPixel = 63.0f;
		b_select = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSizeRec, "Group1/SelectButton", temp));
		b_select->setGroupID(GROUP_ID_ZERO);
		b_select->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onSelectMode, this));

		b_place = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos), buttonSizeRec, "Group1/PlaceButton", temp));
		b_place->setGroupID(GROUP_ID_ZERO);
		b_place->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onPlaceMode, this));

		b_debug = static_cast<CEGUI::ToggleButton*>(m_gui.createWidget("TaharezLook/Checkbox", glm::vec4(0.0f, 0.0f, xPixelPos + 2 * deltaXPixel, yPixelPos), buttonSizeRec, "Group/DebugButton", temp));
		b_debug->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onDebugButtonClicked, this));

	}
	//player, platform OR light
	{
		const int GROUP_ID_ONE = 1;
		glm::vec4 buttonSizeRec(0.0f, 0.0f, 20.0f, 20.0f);

		float xPixelPos = 30.0f;
		float yPixelPos = yPixelOPoint - 85.0f;
		float deltaXPixel = 63.0f;

		b_player = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSizeRec, "Group2/PlayerButton", temp));
		b_player->setGroupID(GROUP_ID_ONE);
		b_player->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onPlayerSelected, this));

		b_platform = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos+ deltaXPixel, yPixelPos), buttonSizeRec, "Group2/PlatformButton", temp));
		b_platform->setGroupID(GROUP_ID_ONE);
		b_platform->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onPlatformSelected, this));

		b_light = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos+2* deltaXPixel, yPixelPos), buttonSizeRec, "Group2/LightButton", temp));
		b_light->setGroupID(GROUP_ID_ONE);
		b_light->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onLightSelected, this));

	}
	//platform widget 
	{
		//physic mode
		{
			const int GROUP_ID_TWO = 2;
			glm::vec4 buttonSizeRec(0.0f, 0.0f, 20.0f, 20.0f);
			float xPixelPos = 30.0f;
			float yPixelPos = yPixelOPoint - 115.0f;
			float deltaXPixel = 50.0f;
			b_rigid = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSizeRec, "Group1/RigidButton", temp));
			b_rigid->setGroupID(GROUP_ID_TWO);
			b_rigid->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onRigidButtonClicked, this));

			b_dynamic = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos), buttonSizeRec, "Group1/DynamicButton", temp));
			b_dynamic->setGroupID(GROUP_ID_TWO);
			b_dynamic->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onDynamicButtonClicked, this));

			b_movable = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + 2 * deltaXPixel, yPixelPos), buttonSizeRec, "Group1/MovableButton", temp));
			b_movable->setGroupID(GROUP_ID_TWO);
			b_movable->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onMovableButtonClicked, this));
			
			b_void = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + 3 * deltaXPixel, yPixelPos), buttonSizeRec, "Group1/VoidButton", temp));
			b_void->setGroupID(GROUP_ID_TWO);
			b_void->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onVoidButtonClicked, this));
		}
		//width, height and angle
		{
			float xPixelPos = 30.0f;
			float yPixelPos = yPixelOPoint - 150.0f;
			float deltaXPixel = 60.0f;
			glm::vec4 sizeRec(0.0f, 0.0f, 55.0f, 25.0f);
			sp_width = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), sizeRec, "Group/WidthSpinner", temp));
			sp_width->setMinimumValue(0.0f);
			sp_width->setMaximumValue(1000.0f);
			sp_width->setStepSize(0.1f);
			sp_width->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_width->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onWidthSpinnerChanged, this));

			sp_height = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos+deltaXPixel, yPixelPos), sizeRec, "Group/HeightSpinner", temp));
			sp_height->setMinimumValue(0.0f);
			sp_height->setMaximumValue(1000.0f);
			sp_height->setStepSize(0.1f);
			sp_height->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_height->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onHeightSpinnerChanged, this));

			sp_angle = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos + 2 * deltaXPixel, yPixelPos ), sizeRec, "Group/AngleSpinner", temp));
			sp_angle->setMinimumValue(0.0f);
			sp_angle->setMaximumValue(360.0f);
			sp_angle->setStepSize(0.1f);
			sp_angle->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_angle->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onAngleSpinnerChanged, this));
		}
		//texture comboBox
		{
			glm::vec4 comboBoxSizeRec(0.0f, 0.0f, 140.0f, 150.0f);
			float xPixelPos3 = 90.0f;
			float yPixelPos3 = yPixelOPoint - 240.0f;
			cb_texture = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.0f, 0.0f, xPixelPos3, yPixelPos3), comboBoxSizeRec, "Group/TextureComboBox", temp));
			cb_texture->subscribeEvent(CEGUI::Combobox::EventTextAccepted, CEGUI::Event::Subscriber(&EditScreen::onTextureInput, this));
			addListToComboBox("Textures/Tiles", cb_texture);
		}
	}
	//light widget
	{
		float xPixelPos = 100.0f;
		float yPixelPos = yPixelOPoint-140.0f;
		glm::vec4 sizeRec(0.0f, 0.0f, 70.0f, 30.0f);
		sp_size = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f,0.0f, xPixelPos, yPixelPos), sizeRec, "Group/SizeSpinner", temp));
		sp_size->setMinimumValue(0.0f);
		sp_size->setMaximumValue(1000.0f);
		sp_size->setStepSize(0.1f);
		sp_size->setTextInputMode(CEGUI::Spinner::FloatingPoint);
		sp_size->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onSizeSpinnerChanged, this));
	}
	//save button and load button and back button
	{
		float xPixelPos = 90.0f;
		float yPixelPos = 30.0f;
		float deltaYPixel = 40.0f;
		glm::vec4 buttonSize(0.0f, 0.0f, 90.0f, 35.0f);
		b_save = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos + 2 * deltaYPixel), buttonSize, "Group/saveButton", temp));
		b_save->setText("Save");
		b_save->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditScreen::onSaveButtonClicked, this));
		
		b_load = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos + deltaYPixel), buttonSize, "Group/loadButton", temp));
		b_load->setText("Load");
		b_load->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditScreen::onLoadButtonClicked, this));

		b_back = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSize, "Group/backButton", temp));
		b_back->setText("Back");
		b_back->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditScreen::onBackButtonClicked, this));
	}
	//file window
	{
		float xPixel = 450.0f;
		float yPixel = 400.0f;
		w_file = static_cast<CEGUI::FrameWindow*>(m_gui.createWidget("TaharezLook/FrameWindow", glm::vec4(0.5f, 0.5f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, xPixel, yPixel), "Root/fileWindow"));
		w_file->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&EditScreen::onfileWindowClose, this));
		w_file->setText("Save Level");
		// Don't let user drag window around
		w_file->setDragMovingEnabled(false);

		float yPixel1 = yPixel - 120.0f;
		cb_text = static_cast<CEGUI::Combobox*>(m_gui.createWidget("TaharezLook/Combobox", glm::vec4(0.5f, 0.0f, 0.0f, yPixel1), glm::vec4(0.0f, 0.0f, 350.0f, 150.0f), "fileWindow/comboBox", static_cast<CEGUI::Window*>(w_file)));
		cb_text->subscribeEvent(CEGUI::Combobox::EventTextAccepted, CEGUI::Event::Subscriber(&EditScreen::onComboBoxEntered, this));
		
		float yPixel2 = yPixel - 350.0f;
		b_OK = static_cast<CEGUI::PushButton*>(m_gui.createWidget("TaharezLook/Button", glm::vec4(0.5f, 0.0f, 0.0f, yPixel2), glm::vec4(0.0f, 0.0f, 45.0f, 30.0f), "fileWindow/OKButton", static_cast<CEGUI::Window*>(w_file)));
		b_OK->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&EditScreen::onOKButtonClicked, this));
		b_OK->setText("OK");
	}
	//initial state
	{//set all slider current value
		s_red->setCurrentValue(255.0f);
		s_green->setCurrentValue(255.0f);
		s_blue->setCurrentValue(255.0f);
		s_alpha->setCurrentValue(255.0f);
		//player, platform, light choose player
		b_player->setSelected(true);
		b_platform->setSelected(false);
		b_light->setSelected(false);
		//place, select choose place
		b_place->setSelected(true);
		b_select->setSelected(false);
		//debug default false
		b_debug->setSelected(false);
		//rigid, dynamic, moveable choose rigid
		b_rigid->setSelected(true);
		b_dynamic->setSelected(false);
		b_movable->setSelected(false);
		//set all spinner current value
		sp_width->setCurrentValue(1.0f);
		sp_height->setCurrentValue(1.0f);
		sp_angle->setCurrentValue(0.0f);
		sp_size->setCurrentValue(10.0f);
		//frame window invisable
		w_file->setVisible(false);
	}

}

void EditScreen::destroy()
{
	m_gui.destroy();
	m_program.dispose();
	m_debugRenderer.destroy();
	m_spriteFont.dispose();
}

void EditScreen::process()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_gui.injectEvent(evnt);
		basicInputProcess(evnt);
		if(evnt.type==SDL_MOUSEWHEEL)
			if(!isMouseInGroup())
			// Linear scaling sucks for mouse wheel zoom so we multiply by getScale() instead.
			m_camera.offsetScale(m_camera.getscale()*evnt.wheel.y * 0.1f);
		if (evnt.type == SDL_KEYDOWN) {
			if (evnt.key.keysym.sym == SDLK_r) {
				m_camera.setscale(1.0f);
			}
		}
	}
}

void EditScreen::update()
{
	m_UIcamera.change();
	m_camera.change();

	if (m_needUpdate && m_selectMode == SelectionMode::SELECT && m_currentObjectIndex != -1) {
		//update any select temp object
		if (m_objectMode == ObjectMode::PLATFORM) {
			Box& b = m_boxes[m_currentObjectIndex];
			//moveObject(b.tempPos);
			b.texture = m_texture;
			b.color = m_color;
			b.dimension = m_dimension;
			b.tempAngle = m_angle;
			b.physicMode = m_physicMode;
		}else if (m_objectMode == ObjectMode::LIGHT) {
			Light& L = m_lights[m_currentObjectIndex];
			//moveObject(L.centerPos);
			L.color = m_color;
			L.size = m_size;
		}
		else {
			//moveObject(m_player.getTempPosReference());
			m_player.tempSetAll(glm::vec4(m_player.getTempPos(), PLAYER_RENDER_DIM), PLAYER_COLLISOIN_DIM, PLAYER_POS_OFFSET, m_playerTexture);
		}
		m_needUpdate = false;
	}
	//m_world->Step(1.0/60.0f,3,8);
}

void EditScreen::draw()
{
	drawScreen();
	drawDebug();
	drawUI();
}
void EditScreen::drawScreen(){
	glm::mat4 camMatrix = m_camera.getcameramatrix();

	m_program.use();
	//texture upload
	glActiveTexture(GL_TEXTURE0);
	GLint samplerLoc = m_program.getuniformposition("mysampler");
	glUniform1i(samplerLoc, 0);
	//camera upload
	GLint camID = m_program.getuniformposition("P");
	glUniformMatrix4fv(camID, 1, GL_FALSE, &camMatrix[0][0]);
	//clear
	m_spriteBatch.begin();
	//draw previews of BOX and PLAYER
	if (m_selectMode == SelectionMode::PLACE && (!isMouseInGroup())) {

		//default aligned position to mouse world coordinates
		glm::vec2 alignedPos = getMouseWorldCords();
		for (auto& B : m_boxes) {
			if (B.isInBox(getMouseWorldCords())) {
				//only change it if is in a box
				glm::vec2 desVec = getMouseWorldCords() - B.tempPos;
				if (desVec.x > 0 && abs(desVec.y) <= desVec.x)
					alignedPos = B.tempPos + glm::vec2((B.dimension.x + m_dimension.x) / 2.0f, 0.0f);
				else if (desVec.x < 0 && abs(desVec.y) <= -desVec.x)
					alignedPos = B.tempPos - glm::vec2((B.dimension.x + m_dimension.x) / 2.0f, 0.0f);
				else if (desVec.y > 0 && abs(desVec.x) <= desVec.y)
					alignedPos = B.tempPos + glm::vec2(0.0f, (B.dimension.y + m_dimension.y) / 2.0f);
				else
					alignedPos = B.tempPos - glm::vec2(0.0f, (B.dimension.y + m_dimension.y) / 2.0f);
				break;
			}
		}

		if (m_objectMode == ObjectMode::PLATFORM) {
			m_tempBox.tempSetAll(glm::vec4(alignedPos,m_dimension), m_angle, m_color, m_texture, m_physicMode);
			m_tempBox.tempDraw(&m_spriteBatch);
		}
		else if (m_objectMode == ObjectMode::PLAYER) {
			m_tempPlayer.tempSetAll(glm::vec4(getMouseWorldCords(), PLAYER_RENDER_DIM), PLAYER_COLLISOIN_DIM, PLAYER_POS_OFFSET, m_playerTexture);
			m_tempPlayer.tempDraw(&m_spriteBatch);
		}
	}
	//m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),glm::vec4(0.0f,0.0f,1.0f,1.0f),m_texture.id,1.0f, Lengine::ColorRGBA8(255, 255, 255, 255));
	//draw BOX and PLAYER existed
	for (int i = 0;i < m_boxes.size();i++) {
		m_boxes[i].tempDraw(&m_spriteBatch);
	}
	if (m_player.getTexture()) {
		//Only draw there is a player 
		m_player.tempDraw(&m_spriteBatch);
	}
	
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_program.unuse();

	//draw all lights
	m_lightPro.use();
	GLint lightCamID = m_lightPro.getuniformposition("P");
	glUniformMatrix4fv(lightCamID, 1, GL_FALSE, &camMatrix[0][0]);
	m_lightSprite.begin();
	//draw preview and lights
	if (m_selectMode == SelectionMode::PLACE && (!isMouseInGroup())) {
		if (m_objectMode == ObjectMode::LIGHT) {
			m_tempLight.tempSetAll( getMouseWorldCords(),m_size,m_color );
			m_tempLight.draw(&m_lightSprite);
		}
	}
	for (int i = 0;i < m_lights.size();i++) {
			m_lights[i].draw(&m_lightSprite);
	}
	m_lightSprite.end();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_lightSprite.renderBatch();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_lightPro.unuse();
}
void EditScreen::drawDebug() {
	glm::mat4 camMatrix = m_camera.getcameramatrix();
	if (m_debuging == true) {
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(1000.0f, 0.0f), Lengine::ColorRGBA8(255, 0, 0, 255));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(-1000.0f, 0.0f), Lengine::ColorRGBA8(255, 0, 0, 100));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f,1000.0f), Lengine::ColorRGBA8(0, 255, 0, 255));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, -1000.0f), Lengine::ColorRGBA8(0, 255, 0, 100));
		for (auto&B : m_boxes) {
			B.tempDebugDraw(&m_debugRenderer);
		}
		for (auto&L : m_lights) {
			L.debugDraw(&m_debugRenderer);
		}
		m_player.tempDebugDraw(&m_debugRenderer);
	}
	if (m_selectMode == SelectionMode::PLACE) {
		if (!isMouseInGroup()) {
			switch (m_objectMode) {
			case ObjectMode::PLATFORM:
				m_tempBox.tempDebugDraw(&m_debugRenderer);
				break;
			case ObjectMode::LIGHT:
				m_tempLight.debugDraw(&m_debugRenderer);
				break;
			case ObjectMode::PLAYER:
				m_tempPlayer.tempDebugDraw(&m_debugRenderer);
				break;
			}
		}
	}
	else {
		if (m_currentObjectIndex != -1) {
			switch (m_objectMode) {
			case ObjectMode::PLATFORM:
				m_boxes[m_currentObjectIndex].tempDebugDraw(&m_debugRenderer, true);
				break;
			case ObjectMode::LIGHT:
				m_lights[m_currentObjectIndex].debugDraw(&m_debugRenderer, true);
				break;
			case ObjectMode::PLAYER:
				m_player.tempDebugDraw(&m_debugRenderer, true);
				break;
			}
		}
	}
	m_debugRenderer.end();
	m_debugRenderer.render(camMatrix, 1.0f);
}
void EditScreen::drawUI() {
	m_program.use();

	glActiveTexture(GL_TEXTURE0);
	GLint samplerLoc = m_program.getuniformposition("mysampler");
	glUniform1i(samplerLoc, 0);

	glm::mat4 UIcamMatrix = m_UIcamera.getcameramatrix();
	GLint camID = m_program.getuniformposition("P");
	glUniformMatrix4fv(camID, 1, GL_FALSE, &UIcamMatrix[0][0]);

	m_spriteBatch.begin();

	//position and size calculate
	float QUAD_SIZE = 50.0f;
	glm::vec2 offset(55.0f, -27.5f);
	glm::vec2 group_LT_WinCords(s_alpha->getPixelPosition().d_x, s_alpha->getPixelPosition().d_y);
	glm::vec2 worldCords = m_UIcamera.wintoworld(group_LT_WinCords);
	worldCords += offset;
	//add to the spritebatch
	m_spriteBatch.draw(glm::vec4(worldCords.x, worldCords.y, QUAD_SIZE, QUAD_SIZE), 1.0f, m_color);
	addLable(static_cast<CEGUI::Window*>(b_select), "Select");
	addLable(static_cast<CEGUI::Window*>(b_place), "Place");
	addLable(static_cast<CEGUI::Window*>(b_debug), "Debug");
	addLable(static_cast<CEGUI::Window*>(b_player), "Player");
	addLable(static_cast<CEGUI::Window*>(b_platform), "Platform");
	addLable(static_cast<CEGUI::Window*>(b_light), "Light");
	addLable(static_cast<CEGUI::Window*>(sp_width), "Width");
	addLable(static_cast<CEGUI::Window*>(sp_height), "Height");
	addLable(static_cast<CEGUI::Window*>(sp_angle), "Angle");
	addLable(static_cast<CEGUI::Window*>(b_rigid), "Rigid");
	addLable(static_cast<CEGUI::Window*>(b_dynamic), "Dynamic");
	addLable(static_cast<CEGUI::Window*>(b_movable), "Movable");
	addLable(static_cast<CEGUI::Window*>(sp_size), "Size");

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_program.unuse();

	m_gui.draw();
}

bool EditScreen::isMouseInGroup() {
	using namespace CEGUI;
	Vector2f mouseCords = m_gui.m_context->getMouseCursor().getPosition();
	Vector2f groupLTCords = m_group->getPixelPosition();
	Sizef groupSize = m_group->getPixelSize();
	Vector2f destVec = mouseCords - groupLTCords;
	if (destVec.d_x > 0 && destVec.d_x < groupSize.d_width && destVec.d_y>0 && destVec.d_y < groupSize.d_height)
		return true;
	else
		return false;
}
glm::vec2 EditScreen::getMouseWorldCords() {
	return m_camera.wintoworld(m_game->getInputManager()->getmousecords());
}
void EditScreen::addLable(CEGUI::Window*widget, const char* text) {
	if (widget->isVisible()) {
		float distance = 7.0f;		//lable has 5pix distance from the wiget
		float scale = 0.6f;
		glm::vec2 pos(widget->getPixelPosition().d_x, widget->getPixelPosition().d_y);
		pos = m_UIcamera.wintoworld(pos);
		pos += glm::vec2(widget->getPixelSize().d_width / 2.0f, distance);
		m_spriteFont.draw(m_spriteBatch, text, pos, glm::vec2(scale), 1.0f,
			Lengine::ColorRGBA8(255, 255, 255, 255), Lengine::Justification::MIDDLE);
	}
}
void EditScreen::addRightLable(CEGUI::Window*widget, const char* text) {
	if (widget->isVisible()) {
		float distance = 7.0f;		//lable has 5pix distance from the wiget
		float scale = 0.6f;
		glm::vec2 pos(widget->getPixelPosition().d_x, widget->getPixelPosition().d_y);
		pos = m_UIcamera.wintoworld(pos);
		pos += glm::vec2(widget->getPixelSize().d_width + distance, -widget->getPixelSize().d_height / 2.0f);
		m_spriteFont.draw(m_spriteBatch, text, pos, glm::vec2(scale), 1.0f, Lengine::ColorRGBA8(255, 255, 255, 255));
	}
}
void EditScreen::setSelectObject(ObjectMode objectMode, int index) {
	b_player->setVisible(false);	b_player->setSelected(false);
	b_platform->setVisible(false);	b_platform->setSelected(false);
	b_light->setVisible(false);		b_light->setSelected(false);
	switch (objectMode)
	{
	case ObjectMode::PLAYER:
		b_player->setVisible(true);
		b_player->setSelected(true);
		break;
	case ObjectMode::PLATFORM:
		b_platform->setVisible(true);
		b_platform->setSelected(true);
		sp_width->setCurrentValue(m_boxes[index].dimension.x);
		sp_height->setCurrentValue(m_boxes[index].dimension.y);
		sp_angle->setCurrentValue(m_boxes[index].tempAngle);
		s_red->setCurrentValue(m_boxes[index].color.r);
		s_green->setCurrentValue(m_boxes[index].color.g);
		s_blue->setCurrentValue(m_boxes[index].color.b);
		s_alpha->setCurrentValue(m_boxes[index].color.a);
		cb_texture->setText(CEGUI::String(m_boxes[index].texture->filePath));
		m_texture = m_boxes[index].texture;
		switch (m_boxes[index].physicMode) {
		case PhysicMode::RIGID:
			b_rigid->setSelected(true);
			break;
		case PhysicMode::DYNAMIC:
			b_dynamic->setSelected(true);
			break;
		case PhysicMode::MOVABLE:
			b_movable->setSelected(true);
			break;
		case  PhysicMode::VOIDSPACE:
			b_void->setSelected(true);
		}
		break;
	case ObjectMode::LIGHT:
		b_light->setVisible(true);
		b_light->setSelected(true);
		sp_size->setCurrentValue(m_lights[index].size);
		s_red->setCurrentValue(m_lights[index].color.r);
		s_green->setCurrentValue(m_lights[index].color.g);
		s_blue->setCurrentValue(m_lights[index].color.b);
		s_alpha->setCurrentValue(m_lights[index].color.a);
		break;
	}
	m_currentObjectIndex = index;
}
//void EditScreen::moveObject(glm::vec2& pos) {
//	const float STEP_LENGTH = 0.01f;
//	Lengine::InputManager* im = m_game->getInputManager();
//	if (im->isKEYdown(SDLK_RIGHT)) {
//		pos.x += STEP_LENGTH;
//	}
//	else if(im->isKEYdown(SDLK_LEFT)) {
//		pos.x -= STEP_LENGTH;
//	}
//	if (im->isKEYdown(SDLK_UP)) {
//		pos.y += STEP_LENGTH;
//	}
//	else if (im->isKEYdown(SDLK_DOWN)) {
//		pos.y -= STEP_LENGTH;
//	}
//}

void EditScreen::addListToComboBox(const char* desDirectory,CEGUI::Combobox* comboBox) {
	//create if not exist, else do nothing
	Lengine::FileIO::createDirectory(desDirectory);

	//get all directory entries and add to the Item list
	std::vector<Lengine::DivEntry> divEntries;
	Lengine::FileIO::getDirectoryEntries(desDirectory, divEntries);

	//clear selection state
	comboBox->clearAllSelections();

	//clear comboBox's all items
	for (auto& I : m_items) {
		comboBox->removeItem(I);
	}
	//clear actual items
	m_items.clear();

	//add the file ones to the item list and add to the comboBox
	for (auto& E : divEntries) {
		if (!E.isDirectory) {
			//erase the path and last '/' character
			E.path.erase(0, std::string(desDirectory).size()+1);
			m_items.emplace_back(new CEGUI::ListboxTextItem(E.path));
			comboBox->addItem(m_items.back());
		}
	}
}
void EditScreen::setPlatformWidgetVisible(bool visibility) {
	b_rigid->setVisible(visibility);
	b_dynamic->setVisible(visibility);
	b_movable->setVisible(visibility);
	b_void->setVisible(visibility);

	sp_width->setVisible(visibility);
	sp_height->setVisible(visibility);
	sp_angle->setVisible(visibility);

	cb_texture->setVisible(visibility);
}
void EditScreen::setLightWidgetVisible(bool visibility) {
	s_alpha->setVisible(visibility);
	sp_size->setVisible(visibility);
}

bool EditScreen::onWindowSelecte(const CEGUI::EventArgs& ea) {
	using namespace CEGUI;
	const WindowEventArgs windowEventArgs = static_cast<const WindowEventArgs&>(ea);
	m_movingWindow = windowEventArgs.window;
	m_mouseUICords = m_gui.m_context->getMouseCursor().getPosition();
	
	Vector2f size(m_movingWindow->getSize().d_width.d_offset, m_movingWindow->getSize().d_height.d_offset);
	Vector2f pos = CoordConverter::screenToWindow(*m_gui.m_root, m_movingWindow->getPosition());
	
	float movableForhead = 30.0f;
	if ((m_mouseUICords.d_y-pos.d_y) < movableForhead) {
		//only move when select through 30 pixel height top part of the window
		m_movingWindow->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&EditScreen::onWindowMove, this));
		m_gui.m_root->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&EditScreen::onWindowMove, this));
	}
	else {
		m_movingWindow = nullptr;
	}

	return true;
}
bool EditScreen::onWindowMove(const CEGUI::EventArgs& ea) {
	using namespace CEGUI;
	Vector2f offset = m_gui.m_context->getMouseCursor().getPosition() - m_mouseUICords;
	m_mouseUICords = m_gui.m_context->getMouseCursor().getPosition();
	UVector2 mouseMove = UVector2(UDim(0.0f, offset.d_x), UDim(0.0f, offset.d_y));
	if (m_movingWindow) {
		m_movingWindow->setPosition(m_movingWindow->getPosition() + mouseMove);
	}
	return true;
}
bool EditScreen::onMouseClicked(const CEGUI::EventArgs& ea) {
	const CEGUI::MouseEventArgs& M = static_cast<const CEGUI::MouseEventArgs&>(ea);
	glm::vec2 m_mouseCords = getMouseWorldCords();

	if (M.button == CEGUI::LeftButton&&m_selectMode == SelectionMode::PLACE) {
			switch (m_objectMode)
			{
			case  ObjectMode::PLATFORM:
				m_boxes.push_back(Box());
				m_boxes.back()= m_tempBox;
				break;
			case  ObjectMode::LIGHT:
				m_lights.push_back(Light{ m_mouseCords, m_size, m_color });
				break;
			case  ObjectMode::PLAYER:
				m_player.tempSetAll(glm::vec4(m_mouseCords, PLAYER_RENDER_DIM), PLAYER_COLLISOIN_DIM, PLAYER_POS_OFFSET, m_playerTexture);
				break;
			}
			return true;
	}
	else if (M.button == CEGUI::RightButton&&m_selectMode == SelectionMode::PLACE) {
		for (auto& B : m_boxes) {
			if (B.isInBox(m_mouseCords)) {
				B = m_boxes.back();
				m_boxes.pop_back();
				break;
			}
		}
		for (auto&L : m_lights) {
			if (L.isInLight(m_mouseCords)) {
				L = m_lights.back();
				m_lights.pop_back();
				break;
			}
		}
		return true;
	}
	return true;
}
bool EditScreen::onMouseDown(const CEGUI::EventArgs& ea) {
	const CEGUI::MouseEventArgs& ma = static_cast<const CEGUI::MouseEventArgs&>(ea);
	if (m_selectMode == SelectionMode::SELECT&&ma.button == CEGUI::LeftButton) {
		m_mouseCords = getMouseWorldCords();
		glm::vec2 tempVec;
		//check if mouse is in Boxes
		for (int i = 0;i < m_boxes.size();i++) {
			Box& B = m_boxes[i];
			if (B.isInBox(m_mouseCords))
			{
				setSelectObject(ObjectMode::PLATFORM, i);
				m_movingObject = true;
				m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&EditScreen::onObjectMove, this));
				return true;
			}
		}
		//check if mouse is in lights
		for (int i = 0;i < m_lights.size();i++) {
			Light& L = m_lights[i];
			tempVec = m_mouseCords - L.centerPos;
			if (glm::length(tempVec) < 1.0f)
			{
				setSelectObject(ObjectMode::LIGHT, i);
				m_movingObject = true;
				m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&EditScreen::onObjectMove, this));
				return true;
			}
		}
		//check if mouse is in player
		if (m_player.isInPlayer(m_mouseCords))
		{
			//there is only one player
			setSelectObject(ObjectMode::PLAYER, 0);
			m_movingObject = true;
			m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&EditScreen::onObjectMove, this));
			return true;
		}
		//if it is in neither
		b_select->setSelected(false);
		b_select->setSelected(true);
		return true;

	}
	return true;
}
bool EditScreen::onObjectMove(const CEGUI::EventArgs& ea) {
	if (m_movingObject) {
		glm::vec2 offset = getMouseWorldCords() - m_mouseCords;
		m_mouseCords = getMouseWorldCords();
		if (m_objectMode == ObjectMode::PLATFORM)
		{
			m_boxes[m_currentObjectIndex].tempPos += offset;
			return true;
		}
		else if (m_objectMode == ObjectMode::LIGHT)
		{
			m_lights[m_currentObjectIndex].centerPos += offset;
			return true;
		}
		else {
			m_player.getTempPosReference() += offset;
			return true;
		}
	}
	return true;
}
bool EditScreen::onMouseUp(const CEGUI::EventArgs& ea) {
	using namespace CEGUI;
	if (m_movingWindow) {
		m_movingWindow = nullptr;
	}
	if (m_movingObject) {
		m_movingObject = false;
	}
	return true;
}

bool EditScreen::onRedSlideChange(const CEGUI::EventArgs& ea)
{
	m_color.r = GLubyte(s_red->getCurrentValue());
	m_needUpdate = true;
	return true;
}
bool EditScreen::onGreenSlideChange(const CEGUI::EventArgs& ea)
{
	m_color.g = GLubyte(s_green->getCurrentValue());
	m_needUpdate = true;
	return true;
}
bool EditScreen::onBlueSlideChange(const CEGUI::EventArgs& ea)
{
	m_color.b = GLubyte(s_blue->getCurrentValue());
	m_needUpdate = true;
	return true;
}
bool EditScreen::onAlphaSliderChange(const CEGUI::EventArgs& ea) {
	m_color.a = GLubyte(s_alpha->getCurrentValue());
	m_needUpdate = true;
	return true;
}

bool EditScreen::onDebugButtonClicked(const CEGUI::EventArgs& ea) {
	m_debuging = b_debug->isSelected();
	return true;
}
bool EditScreen::onSelectMode(const CEGUI::EventArgs& ea) {
	if (b_select->isSelected()) {
		m_selectMode = SelectionMode::SELECT;
		m_currentObjectIndex = -1;
		b_platform->setVisible(false);
		b_player->setVisible(false);
		b_light->setVisible(false);
		setPlatformWidgetVisible(false);
		setLightWidgetVisible(false);
	}
	return true;
}
bool EditScreen::onPlaceMode(const CEGUI::EventArgs& ea) {
	if (b_place->isSelected()) {
		m_selectMode = SelectionMode::PLACE;
		m_currentObjectIndex = -1;
		b_platform->setVisible(true);
		b_player->setVisible(true);
		b_light->setVisible(true);
		b_player->setSelected(true);
	}
	return true;
}

bool EditScreen::onPlayerSelected(const CEGUI::EventArgs& ea) {
	if (b_player->isSelected()) {
		m_objectMode = ObjectMode::PLAYER;
		setPlatformWidgetVisible(false);
		setLightWidgetVisible(false);
	}
	return true;
}
bool EditScreen::onPlatformSelected(const CEGUI::EventArgs& ea) {
	if (b_platform->isSelected()) {
		m_objectMode = ObjectMode::PLATFORM;
		m_color.a = 255;
		setLightWidgetVisible(false);
		setPlatformWidgetVisible(true);
	}
	return true;
}
bool EditScreen::onLightSelected(const CEGUI::EventArgs& ea) {
	if (b_light->isSelected()) {
		m_objectMode = ObjectMode::LIGHT;
		setPlatformWidgetVisible(false);
		setLightWidgetVisible(true);
	}
	return true;
}

bool EditScreen::onRigidButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_rigid->isSelected())
		m_physicMode = PhysicMode::RIGID;
	m_needUpdate = true;
	return true;
}
bool EditScreen::onDynamicButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_dynamic->isSelected())
		m_physicMode = PhysicMode::DYNAMIC;
	m_needUpdate = true;
	return true;
}
bool EditScreen::onMovableButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_movable->isSelected())
		m_physicMode = PhysicMode::MOVABLE;
	m_needUpdate = true;
	return true;
}
bool EditScreen::onVoidButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_void->isSelected())
		m_physicMode = PhysicMode::VOIDSPACE;
	m_needUpdate = true;
	return true;
}

bool EditScreen::onWidthSpinnerChanged(const CEGUI::EventArgs& ea) {
	m_dimension.x = sp_width->getCurrentValue();
	m_needUpdate = true;
	return true;
}
bool EditScreen::onHeightSpinnerChanged(const CEGUI::EventArgs& ea) {
	m_dimension.y = sp_height->getCurrentValue();
	m_needUpdate = true;
	return true;
}
bool EditScreen::onAngleSpinnerChanged(const CEGUI::EventArgs& ea) {
	m_angle = sp_angle->getCurrentValue();
	m_needUpdate = true;
	return true;
}
bool EditScreen::onTextureInput(const CEGUI::EventArgs& ea) {
	std::string texturePath(cb_texture->getText().c_str());
	m_texture = Lengine::textureCache.gettexture("Textures/Tiles/" + texturePath);
	m_needUpdate = true;
	return true;
}
bool EditScreen::onSizeSpinnerChanged(const CEGUI::EventArgs& ea) {
	m_size = sp_size->getCurrentValue();
	m_needUpdate = true;
	return true;
}

bool EditScreen::onSaveButtonClicked(const CEGUI::EventArgs& ea) {
	w_file->setVisible(true);
	m_fileMode = FileMode::SAVE;
	b_select->setSelected(true);
	addListToComboBox("Levels",cb_text);
	return true;
}
bool EditScreen::onLoadButtonClicked(const CEGUI::EventArgs& ea) {
	w_file->setVisible(true);
	m_fileMode = FileMode::LOAD;
	b_select->setSelected(true);
	addListToComboBox("Levels", cb_text);
	return true;
}
bool EditScreen::onBackButtonClicked(const CEGUI::EventArgs& ea) {
	m_previousScreenIndex = MENU_SCREEN;
	m_state = Lengine::ScreenState::TO_PREVIOUS;
	return true;
}

bool EditScreen::onfileWindowClose(const CEGUI::EventArgs& ea) {
	w_file->setVisible(false);
	return true;
}
bool EditScreen::onComboBoxEntered(const CEGUI::EventArgs& ea) {
	onOKButtonClicked(CEGUI::EventArgs());
	return true;
}
bool EditScreen::onOKButtonClicked(const CEGUI::EventArgs& ea) {
	std::string filePath("Levels/" + std::string(cb_text->getText().c_str()));
	switch (m_fileMode)
	{
	case FileMode::SAVE:
		if (m_player.getTexture()) {
			LevelWriterNReader::saveAsText(filePath, m_player, m_boxes, m_lights);
		}
		else {
			std::cout<<"Can't save without Player!\nPlease try angain after adding a player";
		}
		break;
	case FileMode::LOAD:
		LevelWriterNReader::readAsText(filePath, m_player, m_boxes, m_lights);
		break;
	}
	w_file->setVisible(false);
	return true;
}

