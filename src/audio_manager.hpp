#pragma once

#include <string>

#include "../lib/miniaudio/miniaudio.h"

class AudioManager
{
public:

	explicit				AudioManager(const std::string& filename, float volume = 1.0f);

							~AudioManager();

	void					Start();

	void					Stop();

	void					SetTime(uint64_t ms);

	unsigned int			GetTime() const;

	void					SetVolume(float volume);

	float					GetVolume() const;

	void					HangThread();

private:

	struct					DurationData
	{
		unsigned int		frames;
		unsigned int		sampleRate;
		unsigned int		duration_ms;
		unsigned int		position_ms;
	};

	ma_engine				m_engine;
	ma_sound				m_sound;
	DurationData			m_duration;
};