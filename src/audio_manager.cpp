#include <stdexcept>
#include <thread>

#include "audio_manager.hpp"

AudioManager::AudioManager(const std::string& filename)
{
	if (ma_engine_init(NULL, &m_engine) != MA_SUCCESS)
	{
		throw std::runtime_error("Failed to initialise audio engine.");
	}

	if (ma_sound_init_from_file(&m_engine, filename.c_str(), 0, NULL, NULL, &m_sound) != MA_SUCCESS)
	{
		ma_engine_uninit(&m_engine);
		throw std::runtime_error("failed to load audio file");
	}

	ma_uint64 frameCount;
	if (ma_sound_get_length_in_pcm_frames(&m_sound, &frameCount) != MA_SUCCESS)
	{
		ma_sound_uninit(&m_sound);
		ma_engine_uninit(&m_engine);
		throw std::runtime_error("Failed to get audio length");
	}

	ma_uint32 sampleRate = ma_engine_get_sample_rate(&m_engine);

	double duration = (double)frameCount / (double)sampleRate;

	m_duration.sampleRate = sampleRate;
	m_duration.frames = static_cast<unsigned int>(frameCount);
	m_duration.duration_ms = static_cast<unsigned int>(duration * 1000.0 + 1.0); // Round up to the nearest millisecond
	m_duration.position_ms = 0;
}

AudioManager::~AudioManager()
{
	ma_sound_uninit(&m_sound);
	ma_engine_uninit(&m_engine);
}

void AudioManager::Start()
{
	if (ma_sound_start(&m_sound) != MA_SUCCESS)
	{
		throw std::runtime_error("Failed to start sound");
	}
}

void AudioManager::Stop()
{
	if (ma_sound_stop(&m_sound) != MA_SUCCESS)
	{
		throw std::runtime_error("Failed to stop sound");
	}
}

void AudioManager::SetTime(uint64_t ms)
{
	if (ms > m_duration.duration_ms)
	{
		throw std::out_of_range("Time exceeds audio duration");
	}

	uint64_t targetFrame = (ms * static_cast<ma_uint64>(m_duration.sampleRate)) / 1000ULL;

	if (ma_sound_seek_to_pcm_frame(&m_sound, targetFrame) != MA_SUCCESS)
	{
		throw std::runtime_error("Failed to seek");
	}
}

void AudioManager::HangThread()
{
	while (ma_sound_is_playing(&m_sound))
	{
		ma_uint64 cursor;
		ma_sound_get_cursor_in_pcm_frames(&m_sound, &cursor);

		m_duration.position_ms = (1000ULL * cursor) / static_cast<uint64_t>(m_duration.sampleRate);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}