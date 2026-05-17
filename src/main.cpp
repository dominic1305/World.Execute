#include <iostream>
#include <string>
#include <memory>
#include <thread>

#if defined(_WIN32) | defined(_WIN64)
#include <Windows.h>
#endif

#ifdef _DEBUG
#include <unordered_map>
#endif

#include "audio_manager.hpp"
#include "lyric_controller.hpp"

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

template<typename T>
	requires std::same_as<T, std::string&> || std::same_as<T, std::string>
static T lower_str(T&& str)
{
	for (char& c : str)
	{
		c |= (~c & 32ui8);
	}

	return str;
}

#ifdef _DEBUG
struct Settings
{
	SongSection section;
	float volume;
};

static SongSection parse_sect(const std::string& str)
{
	static const std::unordered_map<std::string, SongSection> sect_map
	{
		{ "intro",		SongSection::Intro		},
		{ "verse1",		SongSection::Verse1		},
		{ "prechorus1",	SongSection::PreChorus1	},
		{ "chorus1",	SongSection::Chorus1	},
		{ "verse2",		SongSection::Verse2		},
		{ "prechorus2",	SongSection::PreChorus2	},
		{ "chorus2",	SongSection::Chorus2	},
		{ "postchorus",	SongSection::PostChorus	},
		{ "bridge",		SongSection::Bridge		},
		{ "chorus3",	SongSection::Chorus3	},
		{ "chorus4",	SongSection::Chorus4	},
		{ "outro",		SongSection::Outro		}
	};

	if (!sect_map.contains(str))
	{
		throw std::exception("invalid song section");
	}

	return sect_map.at(str);
}

static Settings parse_startup(std::string& str)
{
	if (lower_str(str).empty())
	{
		throw std::exception("string was empty");
	}

	std::erase_if(str, isspace);
	if (str == "exit")
	{
		throw std::stop_token();
	}

	size_t comma = str.find(',');
	if (comma == std::string::npos)
	{
		throw std::exception("invalid section syntax");
	}

	std::string section_s = str.substr(0, comma);
	std::string volume_s = str.substr(comma + 1);

	SongSection section = parse_sect(section_s);

	if (!volume_s.empty())
	{
		constexpr std::string_view valid = "0123456789.";
		for (const char c : volume_s)
		{
			if (valid.find(c) == std::string::npos)
			{
				throw std::exception("invalid volume argument");
			}
		}
	} else throw std::exception("invalid volume argument");

	float volume = std::stof(volume_s);

	if (volume > 1 || volume < 0)
	{
		throw std::exception("invalid volume");
	}

	return { section, volume };
}
#endif

int main()
{
#if defined(_WIN32) | defined(_WIN64)
	enable_ANSI(); // This is required for windows executables. Windows doesn't enable terminal processing by default
#endif
	std::unique_ptr<AudioManager> audioManager = std::make_unique<AudioManager>("assets/song.mp3");
	std::jthread audioThread([&audioManager](std::stop_token stoken) { audioManager->HangThread(stoken); });

	Lyrics::PrintFromFile("assets/user.txt");

#ifdef _DEBUG
	Settings startup;

	while (true)
	{
		std::cout << Renderer{}("(DEBUG) \"[SECTION], [VOLUME]\" > ");

		std::string buffer;
		getline(std::cin, buffer);

		try
		{
			startup = parse_startup(buffer);
		}
		catch (std::exception& exc)
		{
			Lyrics::ClearTerminal();
			Lyrics::SlowPrint(exc.what(), "[ERROR]", 50_dur, 2, RENDER_RED);
			Lyrics::PrintFromFile("assets/user.txt");
			continue;
		}
		catch (std::stop_token)
		{
			audioThread.request_stop();
			return 0;
		}

		audioManager->SetVolume(startup.volume);

		break; // continue to song
	}
#else
	while (true)
	{
		std::cout << Renderer{}("> ");

		std::string buffer;
		getline(std::cin, buffer);
		lower_str(buffer);

		if (buffer == "exit")
		{
			audioThread.request_stop();
			return 0;
		}

		if (buffer != "run world.exe")
		{
			Lyrics::ClearTerminal();
			Lyrics::PrintFromFile("assets/user.txt");
			continue;
		}

		break; // continue to song
	}
#endif

	Lyrics::AnimatePrint("Song: world.execute(me);", "", 10);
	Lyrics::AnimatePrint("Artist: Mili (JPN)", "", 10);
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	Lyrics::ClearTerminal();

#ifdef _DEBUG
	Lyrics::Execute(audioManager.get(), startup.section);
#else
	Lyrics::Execute(audioManager.get());
#endif

	if (audioThread.joinable())
	{
		audioThread.join();
	}

	return 0;
}