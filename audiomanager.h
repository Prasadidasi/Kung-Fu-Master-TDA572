#pragma once

class AudioManager :public Component{
public:
	//One map each for music and sfx
	std::map<char*, Mix_Chunk*> soundFx;
	std::map<char*, Mix_Music*> musicFx;

	void createSoundFx(char* path, char* soundId) {
		Mix_Chunk* chunk = Mix_LoadWAV(path);
		if (chunk == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Mixer failed the initialization: %s\n", Mix_GetError());
			return;
		}
		soundFx.insert(std::make_pair(soundId, chunk));
	}

	void createMusicFx(char* path, char* musicId) {
		Mix_Music* music = Mix_LoadMUS(path);
		if (music == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Mixer failed the initialization: %s\n", Mix_GetError());
			return;
		}
		musicFx.insert(std::make_pair(musicId, music));
	}

	auto getMusic(char* Id) {
		for (auto Itr : musicFx)
			if (Itr.first == Id)
				return Itr.second;
	}

	auto getSound(char* Id) {
		for (auto itr : soundFx)
			if (itr.first == Id)
				return itr.second;
	}

	void Update(float dt) {
		if (dt <= 0)
			return;

		Engine::KeyStatus keys;
		engine->getKeyStatus(keys);

		if (keys.repeat == false)
			return;

		if (keys.kick)
			Mix_PlayChannel(-1, getSound("KICK"), 0);

		if (keys.punch)
			Mix_PlayChannel(-1, getSound("PUNCH"), 0);
	}

	void playSound(char* Id)
	{
		Mix_PlayChannel(-1, getSound(Id), 0);
	}

	void Destroy() {
		musicFx.clear();
		soundFx.clear();
	}
};
