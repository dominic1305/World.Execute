#include <iostream>
#include <string>
#include <memory>
#include <thread>

#if defined(_WIN32) | defined(_WIN64)
#include <Windows.h>
#endif

#include "audio_manager.hpp"
#include "lyric_controller.hpp"
#include "colour.hpp"

#if defined(_WIN32) | defined(_WIN64)
static void enable_ANSI()
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}
#endif

int main()
{
#if defined(_WIN32) | defined(_WIN64)
	enable_ANSI(); // This is required for windows executables. Windows doesn't enable terminal processing by default
#endif
	std::unique_ptr<AudioManager> audioManager = std::make_unique<AudioManager>("assets/song.mp3");

	std::thread audioThread([&audioManager]() { audioManager->HangThread(); });

#ifdef _DEBUG
	Lyrics::Execute(audioManager.get(), SongSection::Intro);
#else
	Lyrics::Execute(audioManager.get());
#endif

	if (audioThread.joinable())
	{
		audioThread.join();
	}

	return 0;
}