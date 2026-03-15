#include <iostream>
#include <string>
#include <memory>
#include <thread>

#include "audio_manager.hpp"

int main()
{
	std::unique_ptr<AudioManager> audioManager = std::make_unique<AudioManager>("assets/song.mp3");

	std::thread audioThread([&audioManager]() { audioManager->Start(); });

	audioThread.join();

	return 0;
}