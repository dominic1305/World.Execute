#pragma once

#include <string>
#include <initializer_list>
#include <variant>

#include "colour.hpp"
#include "audio_manager.hpp"

struct						_duration
{
	unsigned int			duration;
};

constexpr _duration			operator""_dur(uint64_t duration)
{
	return { static_cast<unsigned int>(duration) };
}

struct						_delay
{
	unsigned int			delay;
};

constexpr _delay			operator""_del(uint64_t delay)
{
	return { static_cast<unsigned int>(delay) };
}

struct						Time
{
	using dur = _duration;
	using del = _delay;

	std::variant<dur, del>	var;

	constexpr				Time(dur duration)
		: var(duration)
	{
	}

	constexpr				Time(del delay)
		: var(delay)
	{
	}

	constexpr unsigned int	operator()(size_t str_length) const
	{
		struct visitor
		{
			unsigned int str_length;

			constexpr unsigned int operator()(dur duration) const
			{
				float delay = static_cast<float>(duration.duration) / str_length;
				unsigned int floored = static_cast<unsigned int>(delay);
				return (delay - floored >= 0.5f) ? floored + 1 : floored;
			}

			constexpr unsigned int operator()(del delay) const
			{
				return delay.delay;
			}
		};

		visitor v{ static_cast<unsigned int>(str_length) };
		return std::visit(v, var);
	}
};

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

namespace				Lyrics
{
	void				ClearTerminal();

	void				PrintFromFile(const std::string& filename, Renderer renderer = {});

	void				SlowPrint(const std::string& str, const std::string& rootname, Time delay_ms, unsigned int newLines, Renderer renderer = {});
	inline void			SlowPrint(const std::string& str, Time delay_ms, unsigned int newLines, Renderer renderer = {});

	void				AnimatePrint(const std::string& actor, const std::string& str, unsigned int loops, Renderer actorRenderer = {}, Renderer strRenderer = {});

	void				SimLoading(const std::string& str, Time delay_ms, unsigned int barLength, Renderer renderer = {});

	std::string			ShuffleWord(const std::string& str);

	void				SimWorld(Renderer renderer = {});

	void				SimWorld2(Renderer renderer = {});

	void				ScrambleTextWall(unsigned int loops, Time duration_ms, std::initializer_list<const char*> bank, Renderer renderer = {});

	std::string			GetBlocks(unsigned int width, Renderer renderer = {});

	std::string			FormatTime();

#ifdef _DEBUG
	void				Execute(AudioManager* audio, SongSection section);
#else
	void				Execute(AudioManager* audio);
#endif
}