#pragma once

#include <string>
#include <initializer_list>

#include "colour.hpp"

struct Renderer
{
	Colour				colour;

	std::string			operator()(const std::string& str);
};

extern const Renderer	RENDER_GREEN;
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

	void				ScrambleTextWall(unsigned int loops, unsigned int duration_ms, std::initializer_list<const char*> bank);

	std::string			GetBlocks(unsigned int width, Renderer renderer = {});

	std::string			FormatTime();
}