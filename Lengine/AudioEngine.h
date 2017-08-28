#pragma once
#include <map>
#include <SDL/SDL_mixer.h>
#include <string>
namespace Lengine {
	class SoundEffect {
	public:
		friend class AudioEngine;
		//-1 loops foever,else replay loos+1 times
		void play(int loops);

	private:
		Mix_Chunk* soundeffect_;
	};

	class Music
	{
	public:
		friend class AudioEngine;
		//-1 loops foever,else replay loos times
		void play(int loops);
		static void pause();
		static void stop();
		static void resume();

	private:
		Mix_Music * music_;

	};

	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();
		//init after SDL_init() everything
		void init();
		void destroy();
		SoundEffect getsoundeffect(const std::string& filepath);
		Music getmusic(const std::string & filepath);
	private:
		std::map<std::string, Mix_Chunk*> soundeffectmap_;
		std::map<std::string, Mix_Music*> Musicmap_;
		int isinitialized_=true;
	};

}