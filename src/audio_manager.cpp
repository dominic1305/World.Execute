#include <stdexcept>
#include <thread>

#include "audio_manager.hpp"

AudioManager::AudioManager(const std::string& filename)
{
	m_result = ma_decoder_init_file(filename.c_str(), NULL, &m_decoder);
    if (m_result != MA_SUCCESS)
    {
        throw std::runtime_error("failed to find file.");
	}

	ma_uint64 frameCount;
    ma_decoder_get_length_in_pcm_frames(&m_decoder, &frameCount);

    ma_uint64 sampleRate = m_decoder.outputSampleRate;

    double duration = (double)frameCount / (double)sampleRate;

	m_duration.isPlaying = false;
	m_duration.frames = static_cast<unsigned int>(frameCount);
	m_duration.sampleRate = static_cast<unsigned int>(sampleRate);
	m_duration.duration_ms = static_cast<unsigned int>(duration * 1000.0 + 1); // Round up to the nearest millisecond
	m_duration.progress_ms.store(0);

    m_deviceConfig = ma_device_config_init(ma_device_type_playback);
    m_deviceConfig.playback.format = m_decoder.outputFormat;
    m_deviceConfig.playback.channels = m_decoder.outputChannels;
    m_deviceConfig.sampleRate = static_cast<unsigned int>(sampleRate);
    m_deviceConfig.dataCallback = data_callback;
    m_deviceConfig.pUserData = &m_decoder;

    if (ma_device_init(NULL, &m_deviceConfig, &m_device) != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to initialize audio device.");
	}
}

AudioManager::~AudioManager()
{
    ma_device_uninit(&m_device);
    ma_decoder_uninit(&m_decoder);
}

void AudioManager::Start()
{
    if (ma_device_start(&m_device) != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to start audio device.");
    }

    m_duration.isPlaying = true;

    while (m_duration.isPlaying && m_duration.progress_ms++ < m_duration.duration_ms)
    {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void AudioManager::Stop()
{
    if (ma_device_stop(&m_device) != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to stop audio device.");
	}

	m_duration.isPlaying = false;
}

void AudioManager::SetTime(unsigned int ms)
{
    if (ms > m_duration.duration_ms)
    {
        throw std::out_of_range("Time exceeds audio duration.");
    }

    auto targetFrame = (ms * m_duration.sampleRate) / 1000;
    if (ma_decoder_seek_to_pcm_frame(&m_decoder, targetFrame) != MA_SUCCESS)
    {
        throw std::runtime_error("Failed to seek to the specified time.");
    }

	m_duration.progress_ms = ms;
}

void AudioManager::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;

    if (pDecoder == NULL)
    {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
}