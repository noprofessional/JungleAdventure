#include "AudioEngine.h"
#include"FatalError.h"
namespace Lengine {

	void SoundEffect::play(int loops) {
		Mix_PlayChannel(-1, soundeffect_, loops);
	}

	void Music::play(int loops) {
		Mix_PlayMusic(music_, loops);
	}
	void Music::pause() {
		Mix_PauseMusic();
	 }
	void Music::stop() {
		Mix_HaltMusic();
	 }
	void Music::resume() {
		Mix_ResumeMusic();
	 }

	AudioEngine::AudioEngine()
	{
	}


	AudioEngine::~AudioEngine()
	{
		destroy();
	}

	void AudioEngine::init()
	{
		if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) == -1)
		{
			fatalError("Mix init Error: " + std::string(Mix_GetError()));
		}
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			fatalError("Mix OpenAudio Error: " + std::string(Mix_GetError()));
		}
		isinitialized_ = true;
	}

	void AudioEngine::destroy()
	{
		//avoid use destroy multiple time
		if (isinitialized_) {
			isinitialized_ = false;

			for(auto& it:soundeffectmap_)
			{
				Mix_FreeChunk(it.second);
			}
			for (auto& it : Musicmap_)
			{
				Mix_FreeMusic(it.second);
			}

			soundeffectmap_.clear();
			Musicmap_.clear();

			Mix_CloseAudio();
			Mix_Quit();
		}
	}

	


	Lengine::SoundEffect AudioEngine::getsoundeffect(const std::string& filepath)
	{
		SoundEffect effect;
		auto it = soundeffectmap_.find(filepath);
		if (it == soundeffectmap_.end()) {
			//have not been loaded
			Mix_Chunk* chunk= Mix_LoadWAV(filepath.c_str());
			if (chunk == nullptr) {
				fatalError("Chunk load Error: " + std::string(Mix_GetError()));
			}
			soundeffectmap_[filepath] = chunk;
			effect.soundeffect_ = chunk;
		}
		else {
			// it has been loaded
			effect.soundeffect_ = it->second;
		}
		return effect;
		
	}

	Lengine::Music AudioEngine::getmusic(const std::string & filepath)
	{
		Music music;
		auto it = Musicmap_.find(filepath);
		if (it == Musicmap_.end()) {
			//have not been loaded
			Mix_Music* musicTemp = Mix_LoadMUS(filepath.c_str());
			if (musicTemp == nullptr) {
				fatalError("Music load Error: " + std::string(Mix_GetError()));
			}
			Musicmap_[filepath] = musicTemp;
			music.music_ = musicTemp;
		}
		else {
			// it has been loaded
			music.music_ = it->second;
		}
		return music;

	}

}