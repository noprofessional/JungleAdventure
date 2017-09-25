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
}

void EditScreen::build()
{
	const int SCREEN_WIDTH = m_game->getWindowPtr()->getscreenwidth();
	const int SCREEN_HEIGHT = m_game->getWindowPtr()->getscreenheight();

	m_backGround = BackGround(Lengine::ColorRGBA8(100, 100, 255, 255), 10);
	m_backGround.setAsCurrent(m_game->getWindowPtr());

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

	m_texture = Lengine::textureCache.gettexture("Textures/white.png");
	m_playerTexture = Lengine::textureCache.gettexture("Textures/Player/IDLE.gif");

	initUI();
	m_hasSelection = false;
	m_stateChanged = true;
}
void EditScreen::initUI() {
	glm::vec2 windowDim(m_game->getWindowPtr()->getscreenwidth(), m_game->getWindowPtr()->getscreenheight());
	m_uvSelector.init(windowDim/2.0f, TextureUVselector::RIGHTTOP);

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
	float groupHeight = 500.0f; float& GH = groupHeight;
	float Margin = 10.0f;
	CEGUI::Window* temp = m_gui.createWidget("TaharezLook/GroupBox",glm::vec4(0.0f, 1.0f, GW / 2.0f + Margin, -GH / 2.0f - Margin),glm::vec4(0.0f, 0.0f, GW, GH), "Group1");
	m_group = static_cast<CEGUI::GroupBox*>(temp);
	m_group->setAlwaysOnTop(false);
	m_group->moveToBack();
	m_group->subscribeEvent(CEGUI::GroupBox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&EditScreen::onWindowSelecte, this));
	m_group->subscribeEvent(CEGUI::GroupBox::EventMouseButtonUp, CEGUI::Event::Subscriber(&EditScreen::onMouseUp, this));

	float yPixelOPoint = 440.0f;
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
	//player, BOX, tile OR light
	{
		const int GROUP_ID_ONE = 1;
		glm::vec4 buttonSizeRec(0.0f, 0.0f, 20.0f, 20.0f);

		float xPixelPos = 23.0f;
		float yPixelPos = yPixelOPoint - 85.0f;
		float deltaXPixel = 48.0f;

		b_player = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSizeRec, "Group2/PlayerButton", temp));
		b_player->setGroupID(GROUP_ID_ONE);
		b_player->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onPlayerSelected, this));

		b_box = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos+ deltaXPixel, yPixelPos), buttonSizeRec, "Group2/BOXButton", temp));
		b_box->setGroupID(GROUP_ID_ONE);
		b_box->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onBoxSelected, this));
	
		b_tile = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + 2*deltaXPixel, yPixelPos), buttonSizeRec, "Group2/TileButton", temp));
		b_tile->setGroupID(GROUP_ID_ONE);
		b_tile->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onTileSelected, this));

		b_light = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos+3* deltaXPixel, yPixelPos), buttonSizeRec, "Group2/LightButton", temp));
		b_light->setGroupID(GROUP_ID_ONE);
		b_light->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onLightSelected, this));

	}
	//BOX widget 
	{
		//physic mode
		{
			const int GROUP_ID_TWO = 2;
			glm::vec4 buttonSizeRec(0.0f, 0.0f, 20.0f, 20.0f);
			float xPixelPos = 25.0f;
			float yPixelPos = yPixelOPoint - 115.0f;
			float deltaXPixel = 63.0f;
			b_rigid = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), buttonSizeRec, "Group1/RigidButton", temp));
			b_rigid->setGroupID(GROUP_ID_TWO);
			b_rigid->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onRigidButtonClicked, this));

			b_dynamic = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos), buttonSizeRec, "Group1/DynamicButton", temp));
			b_dynamic->setGroupID(GROUP_ID_TWO);
			b_dynamic->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onDynamicButtonClicked, this));

			b_movable = static_cast<CEGUI::RadioButton*>(m_gui.createWidget("TaharezLook/RadioButton", glm::vec4(0.0f, 0.0f, xPixelPos + 2 * deltaXPixel, yPixelPos), buttonSizeRec, "Group1/MovableButton", temp));
			b_movable->setGroupID(GROUP_ID_TWO);
			b_movable->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber(&EditScreen::onMovableButtonClicked, this));
			
		}
		//width, height and angle
		{
			float xPixelPos = 50.0f;
			float yPixelPos = yPixelOPoint - 150.0f;
			float deltaXPixel = 75.0f;
			float deltaYPixel = -40.0f;
			glm::vec4 sizeRec(0.0f, 0.0f, 55.0f, 25.0f);
			sp_width = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos, yPixelPos), sizeRec, "Group/WidthSpinner", temp));
			sp_width->setMinimumValue(0.0f);
			sp_width->setMaximumValue(1000.0f);
			sp_width->setStepSize(0.1f);
			sp_width->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_width->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onWidthSpinnerChanged, this));

			sp_height = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos), sizeRec, "Group/HeightSpinner", temp));
			sp_height->setMinimumValue(0.0f);
			sp_height->setMaximumValue(1000.0f);
			sp_height->setStepSize(0.1f);
			sp_height->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_height->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onHeightSpinnerChanged, this));

			sp_angle = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos , yPixelPos+deltaYPixel ), sizeRec, "Group/AngleSpinner", temp));
			sp_angle->setMinimumValue(0.0f);
			sp_angle->setMaximumValue(360.0f);
			sp_angle->setStepSize(0.1f);
			sp_angle->setTextInputMode(CEGUI::Spinner::FloatingPoint);
			sp_angle->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onAngleSpinnerChanged, this));
			
			sp_depth = static_cast<CEGUI::Spinner*>(m_gui.createWidget("TaharezLook/Spinner", glm::vec4(0.0f, 0.0f, xPixelPos + deltaXPixel, yPixelPos+deltaYPixel), sizeRec, "Group/DepthSpinner", temp));
			sp_depth->setMinimumValue(0);
			sp_depth->setMaximumValue(1000);
			sp_depth->setStepSize(1);
			sp_depth->setTextInputMode(CEGUI::Spinner::Integer);
			sp_depth->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&EditScreen::onDepthSpinnerChanged, this));
		}
		//texture comboBox
		{
			glm::vec4 comboBoxSizeRec(0.0f, 0.0f, 140.0f, 150.0f);
			float xPixelPos3 = 90.0f;
			float yPixelPos3 = yPixelOPoint - 290.0f;
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
		//player, BOX, light choose player
		b_player->setSelected(true);
		//place, select choose place
		b_place->setSelected(true);
		//debug default false
		b_debug->setSelected(false);
		//rigid, dynamic, moveable choose rigid
		b_rigid->setSelected(true);
		//set all spinner current value
		sp_width->setCurrentValue(1.0f);
		sp_height->setCurrentValue(1.0f);
		sp_angle->setCurrentValue(0.0f);
		sp_depth->setCurrentValue(1);
		sp_size->setCurrentValue(10.0f);
		//frame window invisable
		w_file->setVisible(false);
	}
}

void EditScreen::destroy()
{
	m_gui.destroy();
	m_program.dispose();
	m_lightPro.dispose();
	m_debugRenderer.destroy();
	m_spriteFont.dispose();
	m_boxes = Boxes();
	m_lights = Lights();
	m_tiles = Tiles();
	m_player = Player();
}

void EditScreen::process()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		m_gui.injectEvent(evnt);
		basicInputProcess(evnt);
		if (evnt.type == SDL_MOUSEWHEEL && !isMouseInUI())
			m_camera.offsetScale(m_camera.getscale()*evnt.wheel.y * 0.1f);
	}
	Lengine::InputManager* im = m_game->getInputManager();
	if (im->isKEYpressed(SDLK_n)) {
		if (m_tiles.hasSelection()) {
			m_tiles.nextCandidate();
			setSelectObject(ObjectMode::TILE);
		}
	}
	
	float xOffset = 0.0f;
	float yOffset = 0.0f;
	if (im->isKEYpressed(SDLK_d)) {
		xOffset = 1.0f;
	}
	else if (im->isKEYpressed(SDLK_a)) {
		xOffset = -1.0f;
	}
	if (im->isKEYpressed(SDLK_w)) {
		yOffset = 1.0f;
	}
	else if (im->isKEYpressed(SDLK_s)) {
		yOffset = -1.0f;
	}
	m_tiles.offsetSelectedPos(xOffset, yOffset);

	if (m_needUpdate && m_selectMode == SelectionMode::SELECT && m_hasSelection) {
		//update any select temp object
		switch (m_objectMode)
		{
		case ObjectMode::BOX:{
			glm::vec4 desRec;
			float angle;
			PhysicalMode physicalMode;
			m_boxes.showSelectedInfo(desRec, angle, physicalMode);
			m_boxes.replaceSelected(Box(glm::vec4(desRec.x,desRec.y,m_dimension), m_angle, m_physicMode));
			break;
		}
		case ObjectMode::TILE: {
			glm::vec4 desRec;
			float depth;
			m_tiles.showSelectedInfo(desRec, glm::vec4(), std::string(), depth);
			m_tiles.replaceSelected(Tile(desRec, m_uvSelector.getUV(), m_texture, m_depth));
			break;
		}
		case ObjectMode::LIGHT: {
			glm::vec2 pos;	float radius;	Lengine::ColorRGBA8 color;
			m_lights.showSelectedInfo(pos, radius, color);
			m_lights.replaceSelected(Light(pos, radius, color));
			break;
		}
		}
		m_needUpdate = false;
	}
}

void EditScreen::update()
{
	changeCamera();
	m_UIcamera.change();
	m_camera.change();
	m_backGround.update();
	//m_world->Step(1.0/60.0f,3,8);
}

void EditScreen::draw()
{
	drawScreen();
	drawDebug();
	drawUI();

}
void EditScreen::drawScreen(){

	m_backGround.draw(&m_program, &m_spriteBatch);

	m_program.use();

	GLint samplerLoc = m_program.getuniformposition("mysampler");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerLoc, 0);
	GLint camID = m_program.getuniformposition("P");
	glm::mat4 camMatrix = m_camera.getcameramatrix();
	glUniformMatrix4fv(camID, 1, GL_FALSE, &camMatrix[0][0]);

	m_spriteBatch.begin(Lengine::GlyphSortType::BACK_TO_FRONT);
	//draw previews of BOX and PLAYER
	if (m_selectMode == SelectionMode::PLACE && (!isMouseInUI())) {

		//default aligned position to mouse world coordinates
		glm::vec2 alignedPos = getMouseWorldCords();
		float x = floor(alignedPos.x*2.0f + 0.5f);
		float y = floor(alignedPos.y*2.0f + 0.5f);
		alignedPos = glm::vec2(x / 2.0f, y / 2.0f);

		if (m_objectMode == ObjectMode::BOX) {
			m_tempBox = Box(glm::vec4(getMouseWorldCords(), m_dimension), m_angle, m_physicMode);
		}
		else if (m_objectMode == ObjectMode::TILE) {
			m_tempTile = Tile(glm::vec4(alignedPos, m_dimension), m_uvSelector.getUV(), m_texture, m_depth);
			m_tempTile.draw(&m_spriteBatch);
		}
		else if (m_objectMode == ObjectMode::PLAYER) {
			m_tempPlayer = Player(glm::vec4(getMouseWorldCords(), PLAYER_RENDER_DIM), PLAYER_COLLISOIN_DIM, PLAYER_POS_OFFSET);
			m_tempPlayer.draw(&m_spriteBatch);
		}
	}

	m_tiles.draw(&m_spriteBatch);
	m_player.draw(&m_spriteBatch);
	
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_program.unuse();


	//draw all lights
	m_lightPro.use();
	GLint lightCamID = m_lightPro.getuniformposition("P");
	glUniformMatrix4fv(lightCamID, 1, GL_FALSE, &camMatrix[0][0]);
	m_lightSprite.begin();
	//draw preview and lights
	if (m_selectMode == SelectionMode::PLACE && (!isMouseInUI())) {
		if (m_objectMode == ObjectMode::LIGHT) {
			m_tempLight = Light(getMouseWorldCords(), m_size, m_color);
			m_tempLight.draw(&m_lightSprite);
		}
	}
	m_lights.draw(&m_lightSprite);
	m_lightSprite.end();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	m_lightSprite.renderBatch();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_lightPro.unuse();
}
void EditScreen::drawDebug() {
	glm::mat4 camMatrix = m_camera.getcameramatrix();
	if (m_debuging == true) {
		m_debugRenderer.drawGrid(glm::vec4(0.0f, 0.0f, 1000.0f, 1000.0f), glm::vec2(1.0f), Lengine::ColorRGBA8(100, 100, 100, 255));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(1000.0f, 0.0f), Lengine::ColorRGBA8(255, 0, 0, 255));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(-1000.0f, 0.0f), Lengine::ColorRGBA8(255, 0, 0, 100));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f,1000.0f), Lengine::ColorRGBA8(0, 255, 0, 255));
		m_debugRenderer.drawLine(glm::vec2(0.0f), glm::vec2(0.0f, -1000.0f), Lengine::ColorRGBA8(0, 255, 0, 100));
	}
	m_player.debugDraw(&m_debugRenderer);
	m_boxes.debugDraw(&m_debugRenderer);
	m_tiles.debugDraw(&m_debugRenderer, m_debuging);
	m_lights.debugDraw(&m_debugRenderer, m_debuging);

	if (m_selectMode == SelectionMode::PLACE) {
		if (!isMouseInUI()) {
			switch (m_objectMode) {
			case ObjectMode::BOX:
				m_tempBox.debugDraw(&m_debugRenderer);
				break;
			case ObjectMode::TILE:
				m_tempTile.debugDraw(&m_debugRenderer);
				break;
			case ObjectMode::LIGHT:
				m_tempLight.debugDraw(&m_debugRenderer);
				break;
			case ObjectMode::PLAYER:
				m_tempPlayer.debugDraw(&m_debugRenderer);
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
	addLable(static_cast<CEGUI::Window*>(b_select), "Select");
	addLable(static_cast<CEGUI::Window*>(b_place), "Place");
	addLable(static_cast<CEGUI::Window*>(b_debug), "Debug");
	addLable(static_cast<CEGUI::Window*>(b_player), "Player");
	addLable(static_cast<CEGUI::Window*>(b_box), "Box");
	addLable(static_cast<CEGUI::Window*>(b_tile), "Tile");
	addLable(static_cast<CEGUI::Window*>(b_light), "Light");
	addLable(static_cast<CEGUI::Window*>(sp_width), "Width");
	addLable(static_cast<CEGUI::Window*>(sp_height), "Height");
	addLable(static_cast<CEGUI::Window*>(sp_angle), "Angle");
	addLable(static_cast<CEGUI::Window*>(sp_depth), "Depth");
	addLable(static_cast<CEGUI::Window*>(b_rigid), "Rigid");
	addLable(static_cast<CEGUI::Window*>(b_dynamic), "Dynamic");
	addLable(static_cast<CEGUI::Window*>(b_movable), "Movable");
	addLable(static_cast<CEGUI::Window*>(sp_size), "Size");

	float QUAD_SIZE = 50.0f;
	glm::vec2 offset(55.0f, -27.5f);
	glm::vec2 group_LT_WinCords(s_alpha->getPixelPosition().d_x, s_alpha->getPixelPosition().d_y);
	glm::vec2 worldCords = m_UIcamera.wintoworld(group_LT_WinCords);
	worldCords += offset;
	m_spriteBatch.draw(glm::vec4(worldCords.x, worldCords.y, QUAD_SIZE, QUAD_SIZE), 1.0f, m_color);

	if (cb_texture->isVisible()) {
		m_uvSelector.drawTexture(&m_spriteBatch);
	}

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_program.unuse();

	if (cb_texture->isVisible()) {
		m_uvSelector.drawLines(&m_debugRenderer);
		m_debugRenderer.end();
		m_debugRenderer.render(m_UIcamera.getcameramatrix(), 1.0f);
	}

	setObjectWidgetVisible();
	m_gui.draw();
}

bool EditScreen::isMouseInUI() {
	glm::vec2 UImouseCords = m_UIcamera.wintoworld(m_game->getInputManager()->getmousecords());
	if (isMouseInGroup() || m_uvSelector.isInSelector(UImouseCords))
		return true;
	else
		return false;
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
		float scale = 0.54f;
		glm::vec2 pos(widget->getPixelPosition().d_x, widget->getPixelPosition().d_y);
		pos = m_UIcamera.wintoworld(pos);
		pos += glm::vec2(widget->getPixelSize().d_width / 2.0f, distance);
		m_spriteFont.draw(m_spriteBatch, text, pos, glm::vec2(scale), 1.0f,
			Lengine::ColorRGBA8(0, 0, 0, 255), Lengine::Justification::MIDDLE);
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
void EditScreen::changeCamera()
{
	const float STEP_LENGTH = 0.1f;
	Lengine::InputManager* im = m_game->getInputManager();
	if (im->isKEYdown(SDLK_RIGHT)) {
		m_camera.offsetPosition(glm::vec2(STEP_LENGTH, 0.0f));
	}
	else if (im->isKEYdown(SDLK_LEFT)) {
		m_camera.offsetPosition(glm::vec2(-STEP_LENGTH, 0.0f));
	}
	if (im->isKEYdown(SDLK_UP)) {
		m_camera.offsetPosition(glm::vec2(0.0f, STEP_LENGTH));
	}
	else if (im->isKEYdown(SDLK_DOWN)) {
		m_camera.offsetPosition(glm::vec2(0.0f, -STEP_LENGTH));
	}
	if (im->isKEYpressed(SDLK_r)) {
		m_camera.setposition(glm::vec2(0.0f));
		m_camera.setscale(32.0f);
	}
}
void EditScreen::setSelectObject(ObjectMode objectMode) {
	switch (objectMode)
	{
	case ObjectMode::PLAYER:
		b_player->setSelected(true);
		break;
	case ObjectMode::BOX: {
		glm::vec4 desRec;
		float angle;
		PhysicalMode physicalMode;
		m_boxes.showSelectedInfo(desRec, angle,physicalMode);

		b_box->setVisible(true);
		b_box->setSelected(true);
		sp_width->setCurrentValue(desRec.z);
		sp_height->setCurrentValue(desRec.w);
		sp_angle->setCurrentValue(angle);
		s_red->setCurrentValue(255);
		s_green->setCurrentValue(255);
		s_blue->setCurrentValue(255);
		s_alpha->setCurrentValue(255);

		switch (physicalMode) {
		case PhysicalMode::RIGID:
			b_rigid->setSelected(true);
			break;
		case PhysicalMode::DYNAMIC:
			b_dynamic->setSelected(true);
			break;
		case PhysicalMode::MOVABLE:
			b_movable->setSelected(true);
			break;
		}

		break;
	}
	case ObjectMode::TILE: {
		glm::vec4 desRec;
		glm::vec4 uvRec;
		std::string texturePath;
		float depth;
		m_tiles.showSelectedInfo(desRec, uvRec, texturePath, depth);

		b_tile->setSelected(true);
		sp_width->setCurrentValue(desRec.z);
		sp_height->setCurrentValue(desRec.w);
		sp_depth->setCurrentValue(depth);
		s_red->setCurrentValue(255);
		s_green->setCurrentValue(255);
		s_blue->setCurrentValue(255);
		s_alpha->setCurrentValue(255);
		cb_texture->setText(CEGUI::String(texturePath));

		m_texture = Lengine::textureCache.getSTClampedTexture(texturePath);
		m_uvSelector.updateTexture(m_texture);
		m_uvSelector.setUV(uvRec);

		break;
	}
	case ObjectMode::LIGHT: {
		b_light->setSelected(true);

		float size;
		Lengine::ColorRGBA8 color;
		m_lights.showSelectedInfo(glm::vec2(), size, color);

		sp_size->setCurrentValue(size);
		s_red->setCurrentValue(color.r);
		s_green->setCurrentValue(color.g);
		s_blue->setCurrentValue(color.b);
		s_alpha->setCurrentValue(color.a);

		break;
	}
	}
	m_hasSelection = true;
	m_movingObject = true;
	m_gui.m_root->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&EditScreen::onObjectMove, this));

}

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
void EditScreen::setObjectWidgetVisible() {
	if (m_stateChanged) {

		b_player->setVisible(false);
		b_box->setVisible(false);
		b_tile->setVisible(false);
		b_light->setVisible(false);
		setBoxWidgetVisible(false);
		setTileWidgetVisible(false);
		setLightWidgetVisible(false);

		switch (m_selectMode)
		{
		case SelectionMode::SELECT:
		{
			if (m_hasSelection) {
				switch (m_objectMode)
				{
				case ObjectMode::PLAYER:
					b_player->setVisible(true);
					break;
				case ObjectMode::BOX:
					b_box->setVisible(true);
					setBoxWidgetVisible(true);
					break;
				case ObjectMode::TILE:
					b_tile->setVisible(true);
					setTileWidgetVisible(true);
					break;
				case ObjectMode::LIGHT:
					b_light->setVisible(true);
					setLightWidgetVisible(true);
					break;
				}
			}
			break;
		}
		case SelectionMode::PLACE:
		{
			b_player->setVisible(true);
			b_box->setVisible(true);
			b_tile->setVisible(true);
			b_light->setVisible(true);
			switch (m_objectMode)
			{
			case ObjectMode::PLAYER:
				b_player->setVisible(true);
				break;
			case ObjectMode::BOX:
				b_box->setVisible(true);
				setBoxWidgetVisible(true);
				break;
			case ObjectMode::TILE:
				b_tile->setVisible(true);
				setTileWidgetVisible(true);
				break;
			case ObjectMode::LIGHT:
				b_light->setVisible(true);
				setLightWidgetVisible(true);
				break;
			}
			break;
		}
		}
		m_stateChanged = false;
	}
}
void EditScreen::setBoxWidgetVisible(bool visibility) {
	b_rigid->setVisible(visibility);
	b_dynamic->setVisible(visibility);
	b_movable->setVisible(visibility);

	sp_width->setVisible(visibility);
	sp_height->setVisible(visibility);
	sp_angle->setVisible(visibility);
}
void EditScreen::setTileWidgetVisible(bool visibility) {
	sp_width->setVisible(visibility);
	sp_height->setVisible(visibility);
	sp_depth->setVisible(visibility);

	cb_texture->setVisible(visibility);
}
void EditScreen::setLightWidgetVisible(bool visibility) {
	s_alpha->setVisible(visibility);
	sp_size->setVisible(visibility);
	if (!visibility)
		s_alpha->setCurrentValue(255);
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

	if (M.button == CEGUI::RightButton) {

		m_boxes.deleteBox(m_mouseCords);
		m_tiles.deleteTile(m_mouseCords);
		m_lights.deleteLight(m_mouseCords);

		return true;
	}
	else if (M.button == CEGUI::LeftButton&&m_selectMode == SelectionMode::PLACE) {
		glm::vec2 UImouseCords = m_UIcamera.wintoworld(m_game->getInputManager()->getmousecords());
		if (m_uvSelector.isInSelector(UImouseCords)) {
			m_uvSelector.updateUV(UImouseCords);
			return true;
		}
		switch (m_objectMode)
			{
			case  ObjectMode::BOX:
				m_boxes.addBox(m_tempBox);
				break;
			case ObjectMode::TILE:
				m_tiles.addTile(m_tempTile);
				break;
			case  ObjectMode::LIGHT:
				m_lights.addLight(m_tempLight);
				break;
			case  ObjectMode::PLAYER:
				m_player = m_tempPlayer;
				break;
			}
			return true;
	}
	return true;
}
bool EditScreen::onMouseDown(const CEGUI::EventArgs& ea) {
	const CEGUI::MouseEventArgs& ma = static_cast<const CEGUI::MouseEventArgs&>(ea);
	if (m_selectMode == SelectionMode::SELECT&&ma.button == CEGUI::LeftButton) {

		Lengine::InputManager* im = m_game->getInputManager();

		bool controlDown = im->isKEYdown(SDLK_LCTRL) || im->isKEYdown(SDLK_RCTRL);
		if (!controlDown) {
			m_boxes.clearSelection();
			m_tiles.clearSelection();
			m_lights.clearSelection();
		}

		m_mouseCords = getMouseWorldCords();

		if (m_boxes.selectBox(m_mouseCords)) {
			setSelectObject(ObjectMode::BOX);
			return true;
		}

		if (m_tiles.selectTile(m_mouseCords, controlDown)) {
			setSelectObject(ObjectMode::TILE);
			return true;
		}
		
		if (m_lights.selectLight(m_mouseCords)) {
			setSelectObject(ObjectMode::LIGHT);
			return true;
		}

		//check if mouse is in player
		if (m_player.selectPlayer(m_mouseCords))
		{
			setSelectObject(ObjectMode::PLAYER);
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
		switch (m_objectMode)
		{
		case ObjectMode::PLAYER:
			m_player.changePos(offset.x, offset.y);
			break;
		case ObjectMode::BOX:
			m_boxes.offsetSelectedPos(offset.x, offset.y);
			break;
		case ObjectMode::LIGHT:
			m_lights.offsetSelectedPos(offset.x, offset.y);
			break;
		default:
			break;
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

		m_boxes.clearSelection();
		m_tiles.clearSelection();
		m_lights.clearSelection();
		m_hasSelection = false;
		m_stateChanged = true;
	}
	return true;
}
bool EditScreen::onPlaceMode(const CEGUI::EventArgs& ea) {
	if (b_place->isSelected()) {
		m_selectMode = SelectionMode::PLACE;
		m_currentObjectIndex = -1;

		m_boxes.clearSelection();
		m_tiles.clearSelection();
		m_lights.clearSelection();
		m_hasSelection = false;
		m_stateChanged = true;
	}
	return true;
}

bool EditScreen::onPlayerSelected(const CEGUI::EventArgs& ea) {
	if (b_player->isSelected()) {
		m_objectMode = ObjectMode::PLAYER;
		m_stateChanged = true;
	}
	return true;
}
bool EditScreen::onBoxSelected(const CEGUI::EventArgs& ea) {
	if (b_box->isSelected()) {
		m_objectMode = ObjectMode::BOX;
		m_stateChanged = true;
	}
	return true;
}
bool EditScreen::onTileSelected(const CEGUI::EventArgs& ea) {
	if (b_tile->isSelected()) {
		m_objectMode = ObjectMode::TILE;
		m_stateChanged = true;
	}
	return true;
}
bool EditScreen::onLightSelected(const CEGUI::EventArgs& ea) {
	if (b_light->isSelected()) {
		m_objectMode = ObjectMode::LIGHT;
		m_stateChanged = true;
	}
	return true;
}

bool EditScreen::onRigidButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_rigid->isSelected())
		m_physicMode = PhysicalMode::RIGID;
	m_needUpdate = true;
	return true;
}
bool EditScreen::onDynamicButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_dynamic->isSelected())
		m_physicMode = PhysicalMode::DYNAMIC;
	m_needUpdate = true;
	return true;
}
bool EditScreen::onMovableButtonClicked(const CEGUI::EventArgs& ea) {
	if (b_movable->isSelected())
		m_physicMode = PhysicalMode::MOVABLE;
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
bool EditScreen::onDepthSpinnerChanged(const CEGUI::EventArgs& ea) {
	m_depth = sp_depth->getCurrentValue();
	m_needUpdate = true;
	return true;
}
bool EditScreen::onTextureInput(const CEGUI::EventArgs& ea) {

	std::string path("Textures/Tiles/");
	path += cb_texture->getText().c_str();

	m_texture = Lengine::textureCache.getSTClampedTexture(path);
	m_uvSelector.updateTexture(m_texture);
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
		LevelWriterNReader::saveAsText(filePath, m_player, m_boxes, m_tiles, m_lights);

		m_tiles.renderToTexture(m_camera);
		LevelWriterNReader::saveAsBinary(filePath + std::string(".level"), m_player, m_boxes, m_tiles, m_lights);

		std::cout << "-------- Saved! --------\n";
		break;
	case FileMode::LOAD:
		LevelWriterNReader::readAsText(filePath, m_player, m_boxes, m_tiles, m_lights);

		std::cout << "-------- "<<filePath<<" Loaded! --------\n";
		break;
	}
	w_file->setVisible(false);
	return true;
}

