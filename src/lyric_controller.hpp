#pragma once

#include <string>
#include <vector>

#include "colour.hpp"

struct Renderer
{
	Colour					colour;

	std::string				operator()(const std::string& str);
};

class Lyrics
{
public:

	static void				ClearTerminal();

	static void				PrintFromFile(const std::string& filename, Renderer renderer = {});

	static void				SlowPrint(const std::string& str, unsigned int delay_ms, unsigned int newLines = 0, Renderer renderer = {});

	static void				AnimatePrint(const std::string& actor, const std::string& str, unsigned int loops, Renderer actorRenderer = {}, Renderer strRenderer = {});

	static void				SimLoading(const std::string& str, unsigned int delay_ms, unsigned int barLength, Renderer renderer = {});

private:

	static std::string		ShuffleWord(const std::string& str);

	static void				SimWorld(Renderer renderer = {});

	static void				SimWorld2(Renderer renderer = {});

	static void				EncryptWall(unsigned int loops, const std::vector<std::string>& bank);

	static std::string		GetBlocks(unsigned int width, Renderer renderer = {});

	static std::string		FormatTime();

};