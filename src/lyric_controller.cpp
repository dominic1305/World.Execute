#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include "lyric_controller.hpp"

#pragma region Static_Functions

static void seed_rand()
{
	static std::time_t rand_seed = std::time(nullptr);	

	if (rand_seed != std::time(nullptr))		// don't reseed if the function is called in the same second
	{											// to prevent resetting back to index 0
		rand_seed = std::time(nullptr);
		std::srand(static_cast<unsigned int>(rand_seed));
	}
}

static std::tm get_local_time() {
	std::time_t t = std::time(nullptr);
	std::tm result{};

#if defined(_WIN32) | defined(_WIN64)
	localtime_s(&result, &t);
#else
	localtime_r(&t, &result);
#endif

	return result;
}

template<typename T>
static std::string& padStart(T&& str, size_t targetLength, char paddingChar)
{
	static_assert(std::is_same_v<T, std::string>, "padStart requires a string");

	if (str.length() >= targetLength)
	{
		return str;
	}

	size_t charsToPad = targetLength - str.length();
	return str.insert(0, charsToPad, paddingChar);
}
#pragma endregion

std::string Renderer::operator()(const std::string& str)
{
	return "\x1b[" + colour() + "m" + str + "\x1b[0m";
}

void Lyrics::ClearTerminal()
{
#if defined(_WIN32) || defined(_WIN64)
	std::system("cls");
#else
	std::system("clear");
#endif
}

void Lyrics::PrintFromFile(const std::string& filename, Renderer renderer)
{
#if _DEBUG
	static bool isFirst = true;
	std::ofstream logFile = (isFirst) ? std::ofstream("printed_files.log", std::ios::trunc) : std::ofstream("printed_files.log", std::ios::app);
	if (logFile.is_open())
	{
		isFirst = false;
		logFile << filename << std::endl;
		logFile.close();
	}
	else
	{
		throw new std::runtime_error("Could not open log file for writing: printed_files.log");
	}
#endif

	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw new std::runtime_error("Could not open file: " + filename);
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::cout << renderer(line) << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	file.close();
}

void Lyrics::SlowPrint(const std::string& str, unsigned int delay_ms, unsigned int newLines, Renderer renderer)
{
	std::cout << renderer("[Console] ");

	for (char c : str)
	{
		std::cout << renderer(std::string(1, c)) << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}

	for (unsigned int i = 0U; i < newLines; ++i)
	{
		std::cout << std::endl;
	}
}

void Lyrics::AnimatePrint(const std::string& actor, const std::string& str, unsigned int loops, Renderer actorRenderer, Renderer strRenderer)
{
	std::cout << '\r';

	for (unsigned int i = 0U; i < loops; ++i)
	{
		std::cout << strRenderer(str) << "" << actorRenderer(ShuffleWord(actor)) << '\r' << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::cout << strRenderer(str) << "" << actorRenderer(actor) << std::flush;
}

void Lyrics::SimLoading(const std::string& str, unsigned int delay_ms, unsigned int barLength, Renderer renderer)
{
	for (unsigned int i = 0U; i <= barLength; i++)
	{
		std::string bar = "[" + std::string(i, '#') + std::string(barLength - i, '-') + "]";
		std::cout << '\r' << renderer(bar + " " + std::to_string(i * 100 / barLength) + "%") << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}
}

std::string Lyrics::ShuffleWord(const std::string& str)
{
	std::string copy = str;

	seed_rand();

	for (int i = (int)str.size() - 1; i > 0; --i)
	{
		int j = std::rand() % (i + 1);
		std::swap(copy[i], copy[j]);
	}

	return copy;
}

void Lyrics::SimWorld(Renderer renderer)
{
	constexpr int TOTAL_BARS = 69;
	constexpr int DELAY_MS = 187;
	constexpr int PHRASE_COUNT = 4;

	std::string phrases[PHRASE_COUNT] = { "Adding. 'You' and 'Me'.", "Generating the Universe", "Adding Star and Moons..", "Crafting the Narrative." };

	seed_rand();

	std::cout << GetBlocks(8, renderer) << renderer("Generating World") << GetBlocks(8, renderer) << std::endl;
	PrintFromFile("assets/generatingWorld1.txt", renderer);
	std::cout << renderer("Seed ID: " + ShuffleWord("642819471920471284")) << std::endl;
	
	for (unsigned int i = 0U; i <= TOTAL_BARS; ++i)
	{
		std::string bar = phrases[std::rand() % PHRASE_COUNT] + " [" + std::string(i, '#') + std::string(TOTAL_BARS - i, '-') + "]";
		int progress = static_cast<int>(static_cast<float>(i) / TOTAL_BARS * 100.0f);

		std::cout << '\r' << renderer(bar + " " + std::to_string(progress) + "%") << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
	}

	std::cout << std::endl;
}

void Lyrics::SimWorld2(Renderer renderer)
{
	constexpr int TOTAL_BARS = 69;
	constexpr int DELAY_MS = 181;
	constexpr int PHRASE_COUNT = 4;
	const std::string phrases[PHRASE_COUNT] = { "4$@(*&$J$@#+$8", "A@$@$%%#$G@)#_", "$%F#&7%.@%=&$?", "Are you there?" };

	seed_rand();

	std::cout << renderer("Lobotomy Corp. INC - 2022") << std::endl;
	PrintFromFile("assets/logoRhine2.txt", renderer);
	std::cout << renderer("4129@$_(921242421\n42@#@!#24124") << std::endl;

	for (unsigned int i = 0U; i <= TOTAL_BARS; ++i)
	{
		std::string bar = phrases[std::rand() % PHRASE_COUNT] + " [" + std::string(i, '#') + std::string(TOTAL_BARS - i, '-') + "]";
		int progress = static_cast<int>(static_cast<float>(i) / TOTAL_BARS * 100.0f);

		std::cout << '\r' << renderer(bar + " " + std::to_string(progress) + "%") << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
	}

	std::cout << std::endl;
}

void Lyrics::EncryptWall(unsigned int loops, const std::vector<std::string>& bank)
{
	int sleepAmount = static_cast<int>(std::ceil(100.0 / loops));

	for (unsigned int i = 0U; i < loops; ++i)
	{
		int count = 0;

		for (int i = 0; i < bank.size(); ++i)
		{
			std::cout << bank[count++] << '\r' << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepAmount));
		}

		std::cout << std::endl;
	}
}

std::string Lyrics::GetBlocks(unsigned int width, Renderer renderer)
{
	if (renderer.colour.m_fg == ColourCode::Default_fg)
	{
		renderer = { Colour(ColourCode::Default_fg, ColourCode::White_bg) };
	}
	else
	{
		renderer = { Colour(ColourCode::Default_fg, static_cast<ColourCode>((int)renderer.colour.m_fg + 10)) };
	}

	return renderer(std::string(width, ' '));
}

std::string Lyrics::FormatTime()
{
	std::tm time = get_local_time();

	bool is_pm = time.tm_hour >= 12;
	int hours = is_pm ? (time.tm_hour - 12 == 0 ? 12 : time.tm_hour - 12) : (time.tm_hour == 0 ? 12 : time.tm_hour);

	return std::to_string(hours) +
		":" +
		padStart(std::to_string(time.tm_min), 2, '0') +
		(is_pm ? " P.M " : " A.M ") +
		padStart(std::to_string(static_cast<unsigned>(time.tm_mday)), 2, '0') +
		"/" +
		padStart(std::to_string(static_cast<unsigned>(time.tm_mon) + 1), 2, '0') + // tm_mon is months since January
		"/" +
		std::to_string(static_cast<int>(time.tm_year) + 1900); // tm_year is years since 1900
}