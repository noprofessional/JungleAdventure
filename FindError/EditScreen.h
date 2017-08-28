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

enum class ObjectMode {
	PLAYER,
	PLATFORM,
	LIGHT,
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

	void addListToComboBox();
	void setPlatformWidgetVisible(bool visible);
	void setLightWidgetVisible(bool visibility);

	//-------- Window related event Funtion --------
	bool onWindowSelecte(const CEGUI::EventArgs& ea);
	bool onWindowMove(const CEGUI::EventArgs& ea);
	bool onMouseClicked(const CEGUI::EventArgs& ea);
	bool onMouseDown(const CEGUI::EventArgs& ea);
	bool onMouseUp(const CEGUI::EventArgs& ea);
	bool onObjectMove(const CEGUI::EventArgs& ea);
	//-------- Slider related event Funtion --------
	bool onRedSlideChange(const CEGUI::EventArgs& ea);
	bool onGreenSlideChange(const CEGUI::EventArgs& ea);
	bool onBlueSlideChange(const CEGUI::EventArgs& ea);
	bool onAlphaSliderChange(const CEGUI::EventArgs& ea);
	//-------- CheckBox related event Function --------
	bool onDebugButtonClicked(const CEGUI::EventArgs& ea);
	bool onSelectMode(const CEGUI::EventArgs& ea);
	bool onPlaceMode(const CEGUI::EventArgs& ea);
	//-------- Radio Button related event Function --------
	bool onPlatformSelected(const CEGUI::EventArgs& ea);
	bool onLightSelected(const CEGUI::EventArgs& ea);
	bool onPlayerSelected(const CEGUI::EventArgs& ea);

	bool onRigidButtonClicked(const CEGUI::EventArgs& ea);
	bool onDynamicButtonClicked(const CEGUI::EventArgs& ea);
	bool onMovableButtonClicked(const CEGUI::EventArgs& ea);
	//-------- Spinner related event Funtion ---------
	bool onWidthSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onHeightSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onAngleSpinnerChanged(const CEGUI::EventArgs& ea);
	bool onSizeSpinnerChanged(const CEGUI::EventArgs& ea);
	//-------- Push Button Clicked Event --------
	bool onSaveButtonClicked(const CEGUI::EventArgs& ea);
	bool onLoadButtonClicked(const CEGUI::EventArgs& ea);
	bool onBackButtonClicked(const CEGUI::EventArgs& ea);
	//-------- File FrameWindow Event ---------
	bool onfileWindowClose(const CEGUI::EventArgs& ea);
	bool onComboBoxEntered(const CEGUI::EventArgs& ea);
	bool onOKButtonClicked(const CEGUI::EventArgs& ea);

	Lengine::GUI m_gui;
	CEGUI::Window* m_movingWindow = nullptr;
	CEGUI::GroupBox* m_group = nullptr;
	//---- 4 slider for color ----
	CEGUI::Slider* s_red= nullptr;
	CEGUI::Slider* s_green = nullptr;
	CEGUI::Slider* s_blue = nullptr;
	CEGUI::Slider* s_alpha = nullptr;
	Lengine::ColorRGBA8 m_color;
	//---- radio buttons ----
	CEGUI::RadioButton * b_select;
	CEGUI::RadioButton* b_place;
	CEGUI::ToggleButton* b_debug;
	CEGUI::RadioButton* b_player;

	CEGUI::RadioButton* b_platform;
	CEGUI::RadioButton* b_rigid;
	CEGUI::RadioButton* b_dynamic;
	CEGUI::RadioButton* b_movable;

	CEGUI::RadioButton* b_light;
	//---- spinner ----
	CEGUI::Spinner* sp_width;
	CEGUI::Spinner* sp_height;
	CEGUI::Spinner* sp_angle;
	CEGUI::Spinner* sp_size;
	//---- button ----
	CEGUI::PushButton* b_save;
	CEGUI::PushButton* b_load;
	CEGUI::PushButton* b_back;
	//---- Frame Window ----
	CEGUI::FrameWindow* w_file;
	CEGUI::Combobox* cb_text;
	CEGUI::PushButton* b_OK;
	std::vector<CEGUI::ListboxTextItem*> m_items;

	//---- Mode ----
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
	std::unique_ptr<b2World> m_world;
	bool m_debuging = false;

	CEGUI::Vector2f m_mouseUICords;
	glm::vec2 m_mouseCords;
	glm::vec4 m_desRec;
	Lengine::GLtexture m_texture;
	Lengine::GLtexture m_playerTexture;
	float m_angle;
	float m_size;
	int m_currentObjectIndex = -1;
	std::vector<Light> m_lights;
	std::vector<Box> m_boxes;
	Box m_tempBox;
	Player m_tempPlayer;
	Player m_player;
	bool m_movingObject =false;
};

