#include "AudioManager.h"
#define CUTE_SOUND_IMPLEMENTATION
#include <cute_sound.h>
#include <Common\Constants.h>

namespace Engine
{
	AudioManager g_AudioManager;

	void AudioManager::Init()
	{
		HWND hwnd = GetConsoleWindow();
		mAudioContext = cs_make_context(hwnd, 44000, 15, 5, 5);

		cs_spawn_mix_thread(mAudioContext);
		cs_thread_sleep_delay(mAudioContext, 5);

		mSounds.reserve(2);
	}
	
	void AudioManager::Destroy()
	{
		cs_shutdown_context(mAudioContext);
		for (auto& sound : mSounds)
		{
			cs_free_sound(&sound.loadedSound);
		}
	}
	
	SoundFile* AudioManager::AddSound(const char* filepath)
	{
		mSounds.push_back(SoundFile());
		SoundFile& sound = mSounds.back();
		sound.playingSound = nullptr;
		sound.loadedSound = cs_load_wav(filepath);
		sound.soundDef = cs_make_def(&sound.loadedSound);
		return &sound;
	}
	
	void AudioManager::PlaySound(SoundFile* sound)
	{
		sound->playingSound = cs_play_sound(mAudioContext, sound->soundDef);
	}
	
	void AudioManager::StopSound(SoundFile* sound)
	{
		cs_stop_sound(sound->playingSound);
		sound->playingSound = nullptr;
	}
	
	void AudioManager::LoopSound(SoundFile* sound, bool loop)
	{
		cs_loop_sound(sound->playingSound, static_cast<int>(loop));
	}

	void AudioManager::SoundSetVolume(SoundFile* sound, float volume)
	{
		cs_set_volume(sound->playingSound, volume, volume);
	}
}

/* EXPORTED INTERFACE */
extern "C"
{
	LAVA_API Engine::SoundFile* AddSound_Native(const char* path)
	{
		return Engine::g_AudioManager.AddSound(path);
	}

	LAVA_API void PlaySound_Native(Engine::SoundFile* sound)
	{
		Engine::g_AudioManager.PlaySound(sound);
	}

	LAVA_API void StopSound_Native(Engine::SoundFile* sound)
	{
		Engine::g_AudioManager.StopSound(sound);
	}

	LAVA_API void LoopSound_Native(Engine::SoundFile* sound, bool loop)
	{
		if (sound->playingSound != nullptr)
			Engine::g_AudioManager.LoopSound(sound, loop);
		else
			sound->soundDef.looped = static_cast<int>(loop);
	}

	LAVA_API void SoundSetVolume_Native(Engine::SoundFile* sound, float volume)
	{
		if (sound->playingSound != nullptr)
			Engine::g_AudioManager.SoundSetVolume(sound, volume);
		else
			sound->soundDef.volume_left = sound->soundDef.volume_right = volume;
	}
}