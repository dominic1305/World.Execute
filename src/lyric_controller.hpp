#pragma once

#include <string>
#include <initializer_list>

#include "colour.hpp"
#include "audio_manager.hpp"

#ifdef _DEBUG
enum class SongSection : char
{
	Intro,
	Verse1,
	PreChorus1,
	Chorus1,
	Verse2,
	PreChorus2,
	Chorus2,
	PostChorus,
	Bridge,
	Chorus3,
	Chorus4,
	Outro
};
#endif

struct Renderer
{
	Colour				colour;

	std::string			operator()(const std::string& str) const;
};

extern const Renderer	RENDER_GREEN;
extern const Renderer	RENDER_RED;
extern const Renderer	RENDER_HIGHLIGHT;

namespace Lyrics
{
	void				ClearTerminal();

	void				PrintFromFile(const std::string& filename, Renderer renderer = {});

	void				SlowPrint(const std::string& str, unsigned int delay_ms, unsigned int newLines = 0, Renderer renderer = {});

	void				AnimatePrint(const std::string& actor, const std::string& str, unsigned int loops, Renderer actorRenderer = {}, Renderer strRenderer = {});

	void				SimLoading(const std::string& str, unsigned int delay_ms, unsigned int barLength, Renderer renderer = {});

	std::string			ShuffleWord(const std::string& str);

	void				SimWorld(Renderer renderer = {});

	void				SimWorld2(Renderer renderer = {});

	void				ScrambleTextWall(unsigned int loops, unsigned int duration_ms, std::initializer_list<const char*> bank, Renderer renderer = {});

	std::string			GetBlocks(unsigned int width, Renderer renderer = {});

	std::string			FormatTime();

#ifdef _DEBUG
	void				Execute(AudioManager* audio, SongSection section);
#else
	void				Execute(AudioManager* audio);
#endif
}