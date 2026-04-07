#pragma once

#include <string>
#include <atomic>

#include "../lib/miniaudio.h"

class AudioManager
{
public:

	explicit				AudioManager(const std::string& filename);

							~AudioManager();

	void					Start();

	void					Stop();

	void					SetTime(unsigned int ms);

private:

	struct					DurationData
	{
		std::atomic_bool	isPlaying;
		unsigned int		frames;
		unsigned int		sampleRate;
		unsigned int		duration_ms;
		std::atomic_uint	progress_ms;
	};

	static void				data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	ma_result				m_result;
	DurationData			m_duration;
	ma_decoder				m_decoder;
	ma_device_config		m_deviceConfig;
	ma_device				m_device;

};