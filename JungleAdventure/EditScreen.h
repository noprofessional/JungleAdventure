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
#include "Tile.h"
#include "BackGround.h"
#include "TextureUVselector.h"
enum class ObjectMode {
	PLAYER,
	BOX,
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
	bool isMouseInUI();
	bool isMouseInGroup();
	glm::vec2 getMouseWorldCords();
	void addLable(CEGUI::Window*widget,const char* text);
	void addRightLable(CEGUI::Window*widget, const char* text);
	void changeCamera();

	void setSelectObject(ObjectMode objectMode);
	void addListToComboBox(const char* desDirectory, CEGUI::Combobox* comboBox);
	void setObjectWidgetVisible();
	void setBoxWidgetVisible(bool visibility);
	void setTileWidgetVisible(bool visibility);
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
	bool onBoxSelected(const CEGUI::EventArgs& ea);
	bool onTileSelected(const CEGUI::EventArgs& ea);
	bool onLightSelected(const CEGUI::EventArgs& ea);
	bool onPlayerSelected(const CEGUI::EventArgs& ea);
	bool onRigidButtonClicked(const CEGUI::EventArgs& ea);
	bool onDynamicButtonClicked(const CEGUI::EventArgs& ea);
	bool onMovableButtonClicked(const CEGUI::EventArgs& ea);
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
	CEGUI::RadioButton* b_box = nullptr;
	CEGUI::RadioButton* b_tile = nullptr;
	CEGUI::RadioButton* b_light = nullptr;
	CEGUI::RadioButton* b_rigid = nullptr;
	CEGUI::RadioButton* b_dynamic = nullptr;
	CEGUI::RadioButton* b_movable = nullptr;
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
	PhysicalMode m_physicMode = PhysicalMode::RIGID;
	FileMode m_fileMode = FileMode::SAVE;

	Lengine::Camera m_UIcamera;
	Lengine::Camera m_camera;
	Lengine::Camera m_frameCamera;
	Lengine::ShaderPro m_program;
	Lengine::ShaderPro m_lightPro;
	Lengine::SpriteBatch m_spriteBatch;
	Lengine::SpriteBatch m_secondBatch;
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
	TextureUVselector m_uvSelector;
	Lengine::GLtexture* m_playerTexture;

	int m_currentObjectIndex = -1;

	Box m_tempBox;
	Boxes m_boxes;

	Tile m_tempTile;
	Tiles m_tiles;

	Player m_tempPlayer;
	Player m_player;

	Lights m_lights;
	Light m_tempLight;

	bool m_hasSelection;
	bool m_stateChanged;
	BackGround m_backGround;

};

