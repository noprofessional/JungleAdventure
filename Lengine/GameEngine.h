#pragma once
namespace Lengine {
	class GameEngine
	{
	public:
		//initialize SDL and Glew
		static void init();
		static bool engineInit;
	};

}