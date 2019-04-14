#pragma once

#include <vector>
#include <cute_sound.h>

namespace Engine
{
	struct SoundFile
	{
		cs_loaded_sound_t loadedSound;
		cs_play_sound_def_t soundDef;
		cs_playing_sound_t* playingSound;
	};

	class AudioManager
	{
	public:
		void Init();
		void Destroy();

		// Add a new sound in wav format
		SoundFile* AddSound(const char* filepath);

		void PlaySound(SoundFile* sound);
		void StopSound(SoundFile* sound);

		// Set loop of playing sound
		void LoopSound(SoundFile* sound, bool loop);
		// Set volume of playing sound
		void SoundSetVolume(SoundFile* sound, float volume);

	private:
		cs_context_t* mAudioContext;
		std::vector<SoundFile> mSounds;
	};

	extern AudioManager g_AudioManager;
}