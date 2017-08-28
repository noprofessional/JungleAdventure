#pragma once
#include<CEGUI/CEGUI.h>
#include<CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include<glm/glm.hpp>
#include<SDL/SDL_events.h>
namespace Lengine {
	class GUI
	{
	public:
		void init(const std::string& resourceDirectory);
		void destroy();
		void update();
		void draw();
		void loadScheme(const std::string& schemeFile);
		void setFont(const std::string& fontFile);
		void setMouseCursor(const std::string & imageFile);
		void showMouseCursor();
		void hideMouseCursor();

		//actions can be subscribed to buttons 

		void injectEvent(SDL_Event& evnt);
		CEGUI::Window* createWidget(
			const std::string& type,
			const glm::vec4& posPercNpix,
			const glm::vec4& sizePercNpix,
			const std::string& name = "", CEGUI::Window* parent = nullptr);


		CEGUI::GUIContext * m_context=nullptr;
		static CEGUI::OpenGL3Renderer * m_renderer;
		CEGUI::Window* m_root;
	private:
		unsigned int m_lastTime;
	};

}