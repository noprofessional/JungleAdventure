#pragma once
#include<Lengine/IScreen.h>
#include<Lengine/GUI.h>
#include<Lengine/ShaderPro.h>
#include<Lengine/SpriteBatch.h>
#include<Lengine/SpriteFont.h>
#include<Lengine/Camera.h>
#include<Lengine/DebugRender.h>
#include"Box.h"
#include"Light.h"
#include"Player.h"
#include "BackGround.h"
enum class ObjectMode {
	PLAYER,
	PLATFORM,
	TILE,
	LIGHT
};
enum class SelectionMode {
	SELECT,
	PLACE
};
enum class FileMode {
	SAVE,
	LOAD
};

class EditScreen:public Lengine::IScreen
{
public:
	EditScreen(Lengine::IMainGame* ownergame);
	~EditScreen();
	void build() override;
	void initUI();
	void destroy() override;

	void process() override;
	void update() override;
	void draw() override;
	void drawUI();
	void drawScreen();
	void drawDebug();
private:
	bool isMouseInGroup();
	glm::vec2 getMouseWorldCords();
	void addLable(CEGUI::Window*widget,const char* text);
	void addRightLable(CEGUI::Window*widget, const char* text);
	void setSelectObject(ObjectMode objectMode, int index);
	void moveObject(glm::vec2& pos);

	void addListToComboBox(const char* desDirectory, CEGUI::Combobox* comboBox);
	void setPlatformWidgetVisible(bool visible);
	void setLightWidgetVisible(bool visibility);

	//////////////////////////////////////////////////////////////////////////
	//Event Call Back Functions
	//////////////////////////////////////////////////////////////////////////
	bool onWindowSelecte(const CEGUI::EventArgs& ea);
	bool onWindowMove(const CEGUI::EventArgs& ea);
	bool onMouseClicked(const CEGUI::EventArgs& ea);
	bool onMouseDown(const CEGUI::EventArgs& ea);
	bool onMouseUp(const CEGUI::EventArgs& ea);
	bool onObjectMove(const CEGUI::EventArgs& ea);
	bool onRedSlideChange(const CEGUI::EventArgs& ea);
	bool onGreenSlideChange(const CEGUI::EventArgs& ea);
	bool onBlueSlideChange(const CEGUI::EventArgs& ea);
	bool onAlphaSliderChange(const CEGUI::EventArgs& ea);
	bool onDebugButtonClicked(const CEGUI::EventArgs& ea);
	bool onSelectMode(const CEGUI::EventArgs& ea);
	bool onPlaceMode(const CEGUI::EventArgs& ea);
	bool onPlatformSelected(const CEGUI::EventArgs& ea);
	bool onLightSelected(const CEGUI::EventArgs& ea);
	bool onPlayerSelected(const CEGUI::EventArgs& ea);
	bool onRigidButtonClicked(const CEGUI::EventArgs& ea);
	bool onDynamicButtonClicked(const CEGUI::EventArgs& ea);
	bool onMovableButtonClicked(const CEGUI::EventArgs& ea);
	bool onVoidButtonClicked(const CEGUI::EventArgs& ea);
	bool onWidthSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onHeightSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onAngleSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onDepthSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onTextureInput(const CEGUI::EventArgs& ea);
	bool onSizeSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onSaveButtonClicked(const CEGUI::EventArgs& ea);
	bool onLoadButtonClicked(const CEGUI::EventArgs& ea);
	bool onBackButtonClicked(const CEGUI::EventArgs& ea);
	bool onfileWindowClose(const CEGUI::EventArgs& ea);
	bool onComboBoxEntered(const CEGUI::EventArgs& ea);
	bool onOKButtonClicked(const CEGUI::EventArgs& ea);

	//////////////////////////////////////////////////////////////////////////
	//CEGUI Widget Handlers
	//////////////////////////////////////////////////////////////////////////
	Lengine::GUI m_gui;
	CEGUI::GroupBox* m_group = nullptr;
	CEGUI::Slider* s_red= nullptr;
	CEGUI::Slider* s_green = nullptr;
	CEGUI::Slider* s_blue = nullptr;
	CEGUI::Slider* s_alpha = nullptr;
	CEGUI::RadioButton * b_select = nullptr;
	CEGUI::RadioButton* b_place = nullptr;
	CEGUI::ToggleButton* b_debug = nullptr;
	CEGUI::RadioButton* b_player = nullptr;
	CEGUI::RadioButton* b_platform = nullptr;
	CEGUI::RadioButton* b_light = nullptr;
	CEGUI::RadioButton* b_rigid = nullptr;
	CEGUI::RadioButton* b_dynamic = nullptr;
	CEGUI::RadioButton* b_movable = nullptr;
	CEGUI::RadioButton* b_void = nullptr;
	CEGUI::Spinner* sp_width = nullptr;
	CEGUI::Spinner* sp_height = nullptr;
	CEGUI::Spinner* sp_angle = nullptr;
	CEGUI::Spinner* sp_depth = nullptr;
	CEGUI::Combobox* cb_texture = nullptr;
	CEGUI::Spinner* sp_size = nullptr;
	CEGUI::PushButton* b_save = nullptr;
	CEGUI::PushButton* b_load = nullptr;
	CEGUI::PushButton* b_back = nullptr;
	CEGUI::FrameWindow* w_file = nullptr;
	CEGUI::Combobox* cb_text = nullptr;
	CEGUI::PushButton* b_OK = nullptr;
	CEGUI::Window* m_movingWindow = nullptr;
	std::vector<CEGUI::ListboxTextItem*> m_items;

	ObjectMode m_objectMode = ObjectMode::PLAYER;
	SelectionMode m_selectMode = SelectionMode::PLACE;
	PhysicMode m_physicMode = PhysicMode::RIGID;
	FileMode m_fileMode = FileMode::SAVE;

	Lengine::Camera m_UIcamera;
	Lengine::Camera m_camera;
	Lengine::ShaderPro m_program;
	Lengine::ShaderPro m_lightPro;
	Lengine::SpriteBatch m_spriteBatch;
	Lengine::SpriteBatch m_lightSprite;
	Lengine::SpriteFont m_spriteFont;
	Lengine::DebugRender m_debugRenderer;

	bool m_debuging = false;
	bool m_movingObject = false;
	bool m_needUpdate = false;
	glm::vec2 m_mouseCords;
	CEGUI::Vector2f m_mouseUICords;
	glm::vec2 m_dimension;
	float m_angle;
	float m_depth;
	float m_size;
	Lengine::ColorRGBA8 m_color;
	Lengine::GLtexture* m_texture;
	Lengine::GLtexture* m_playerTexture;

	int m_currentObjectIndex = -1;
	Player m_player;
	std::vector<Box> m_boxes;
	std::vector<Light> m_lights;

	Player m_tempPlayer;
	Box m_tempBox;
	Light m_tempLight;
	BackGround m_backGround;

};

