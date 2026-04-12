#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#ifdef _DEBUG
#include <unordered_map>
#include <list>
#include <filesystem>
#endif

#include "lyric_controller.hpp"

#pragma region Static_Functions
static bool atLineStart = true;

#ifdef _DEBUG
static std::list<std::string> used_files{};

static inline void log_asset(const std::string& filename)
{
	used_files.push_back(filename);
}

static void save_used_files()
{
	std::cout << Renderer{ ColourCode::Cyan_fg }("\nSaving used assets...");

	std::ofstream log_used("printed_files.log", std::ios::trunc);
	if (log_used.is_open())
	{
		for (const std::string& filename : used_files)
		{
			log_used << filename << std::endl;
		}

		log_used.close();
	}

	std::ofstream log_all("used_files.log", std::ios::trunc);
	if (log_all.is_open())
	{
		for (const auto& file : std::filesystem::directory_iterator("assets/"))
		{
			std::string filename = file.path().string();

			if (filename == "assets/song.mp3") continue;

			bool is_used = false;

			for (const std::string& used : used_files)
			{
				if (filename == used)
				{
					is_used = true;
					break;
				}
			}

			log_all << filename << " = " << (is_used ? "true" : "false") << std::endl;
		}
	}

	std::cout << Renderer{ ColourCode::Cyan_fg }("\rUsed assets saved to \"used_files.log\"");
}

static std::unordered_map<SongSection, unsigned int> song_positions{};

static unsigned int parse_timestamp(std::string str)
{
	if (str.empty()) return 0;
	
	size_t idx = str.find('=');
	if (idx != std::string::npos)
	{
		str.erase(0, idx + 1);
	}
	else return 0;

	return std::stoi(str);
}

static inline void log_timestamp(SongSection section, unsigned int timestamp)
{
	song_positions.at(section) = timestamp;
}

static bool load_song_positions()
{
	std::ifstream file("timestamps.log");

	constexpr int BUCKET_COUNT = static_cast<int>(SongSection::Outro) + 1;
	song_positions.reserve(BUCKET_COUNT);

	if (file.is_open())
	{
		std::string buffer;
		for (int i = 0; i < BUCKET_COUNT; ++i)
		{
			std::getline(file, buffer);
			song_positions.emplace(static_cast<SongSection>(i), parse_timestamp(buffer));
		}

		file.close();
		return true;
	}
	else
	{
		for (int i = 0; i < BUCKET_COUNT; ++i)
		{
			song_positions.emplace(static_cast<SongSection>(i), 0UL);
		}

		file.close();
		return false;
	}
}

static void save_song_positions()
{
	std::cout << Renderer{ ColourCode::Cyan_fg }("\nSaving song positions...");

	std::ofstream file("timestamps.log", std::ios::trunc);

	if (file.is_open())
	{
		for (int i = 0; i < song_positions.size(); ++i)
		{
			file << i << '=' << song_positions.at(static_cast<SongSection>(i)) << std::endl;
		}

		file.close();
	}

	std::cout << Renderer{ ColourCode::Cyan_fg }("\rSong positions saved to \"timestamps.log\"");
}
#endif

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
	static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::string&>, "padStart requires a string");

	if (str.length() >= targetLength)
	{
		return str;
	}

	size_t charsToPad = targetLength - str.length();
	return str.insert(0, charsToPad, paddingChar);
}
#pragma endregion

std::string Renderer::operator()(const std::string& str) const
{
	return "\x1b[" + colour() + "m" + str + "\x1b[0m";
}

const Renderer RENDER_GREEN = { ColourCode::Green_fg };
const Renderer RENDER_RED = { ColourCode::Red_fg };
const Renderer RENDER_HIGHLIGHT = { { .m_fg = ColourCode::Yellow_fg, .m_dark = true, .m_underline = true } };

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
#ifdef _DEBUG
	log_asset(filename);
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
	if (atLineStart)
	{
		std::cout << renderer("[Console] ");
		atLineStart = false;
	}

	for (char c : str)
	{
		std::cout << renderer(std::string(1, c)) << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}

	for (unsigned int i = 0U; i < newLines; ++i)
	{
		std::cout << std::endl;
		atLineStart = true;
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

	std::cout << strRenderer(str) << "" << actorRenderer(actor) << std::endl;
	atLineStart = true;
}

void Lyrics::SimLoading(const std::string& str, unsigned int delay_ms, unsigned int barLength, Renderer renderer)
{
	for (unsigned int i = 0U; i <= barLength; i++)
	{
		std::string bar = "[" + std::string(i, '#') + std::string(barLength - i, '-') + "]";
		std::cout << '\r' << renderer(bar + " " + std::to_string(i * 100 / barLength) + "% ") << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
	}

	std::cout << renderer(str) << std::endl;
	atLineStart = true;
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
	atLineStart = true;
}

void Lyrics::SimWorld2(Renderer renderer)
{
	constexpr int TOTAL_BARS = 69;
	constexpr int DELAY_MS = 190;
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
	atLineStart = true;
}

void Lyrics::ScrambleTextWall(unsigned int loops, unsigned int duration_ms, std::initializer_list<const char*> bank, Renderer renderer)
{
	unsigned int sleepAmount = static_cast<unsigned int>(std::round(duration_ms / loops / bank.size()));

	for (unsigned int i = 0U; i < loops; ++i)
	{
		for (int ii = 0; ii < bank.size(); ++ii)
		{
			std::cout << renderer(*(bank.begin() + ii)) << '\r' << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepAmount));
		}
		
		std::cout << std::endl;
	}
}

std::string Lyrics::GetBlocks(unsigned int width, Renderer renderer)
{
	if (renderer.colour.m_fg == ColourCode::Default_fg)
	{
		renderer.colour.m_bg = ColourCode::White_bg;
	}
	else
	{
		renderer.colour.m_bg = static_cast<ColourCode>(static_cast<char>(renderer.colour.m_fg) + 10);
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

#if false // This purely exists to silence the linter. Just ignore this
void Lyrics::Execute(AudioManager* audio, SongSection section) { }
#endif

#ifdef _DEBUG
void Lyrics::Execute(AudioManager* audio, SongSection section)
#else
void Lyrics::Execute(AudioManager* audio)
#endif
{
#ifdef _DEBUG
	bool isloaded = load_song_positions();
	bool isSectionSkipped = false;

	audio->SetTime(song_positions.at(section)); // If not loaded, all values in `song_position` are 0 so no need to set time to Intro
	audio->Start();

	if (!isloaded) { goto Intro; }
	if (section != SongSection::Intro) isSectionSkipped = true;

	switch (section)
	{
		case SongSection::Intro:		goto Intro;
		case SongSection::Verse1:		goto Verse1;
		case SongSection::PreChorus1:	goto PreChorus1;
		case SongSection::Chorus1:		goto Chorus1;
		case SongSection::Verse2:		goto Verse2;
		case SongSection::PreChorus2:	goto PreChorus2;
		case SongSection::Chorus2:		goto Chorus2;
		case SongSection::PostChorus:	goto PostChorus;
		case SongSection::Bridge:		goto Bridge;
		case SongSection::Chorus3:		goto Chorus3;
		case SongSection::Chorus4:		goto Chorus4;
		case SongSection::Outro:		goto Outro;
	}
#else
	audio->Start();
#endif

#pragma region Intro
#ifdef _DEBUG
	Intro:
	log_timestamp(SongSection::Intro, audio->GetTime());
#endif
	SlowPrint("Switch on the Power Line", 50, 1, RENDER_GREEN);
	PrintFromFile("assets/logoRhine.txt");
	SlowPrint("Remember to put on -- P R O T E C T I O N", 50, 3, RENDER_GREEN);

	std::cout << Renderer{ ColourCode::Yellow_fg }(">EncryptEnigma();\n");
	SimLoading("Encryption Set\n", 20, 10);

	SlowPrint("Lay down you pieces and let's begin ", 50, 0, RENDER_GREEN);
	SlowPrint("ObjectCreation();", 50, 1, { ColourCode::Yellow_fg });

	AnimatePrint("[Console] Fill in my data.", "", 5, RENDER_GREEN);
	PrintFromFile("assets/createObject.txt");
	SlowPrint("Parameters. ", 70, 1, RENDER_GREEN);
	ScrambleTextWall(3, 600, {
		"bVJijcss",
		"ijcssjzpTjH9no@K-emz",
		"1+Rza-<cT+G\"sC6IhJci!b ",
		"8V!g8WFkHzm<",
		"9no@K-emz",
		"56.23.15  >> Rotors ON "
		});
	SlowPrint("Initialisation.", 60, 1, RENDER_GREEN);
	PrintFromFile("assets/getError.txt", RENDER_RED);
	AnimatePrint("world.toggleValidity(false)", "", 3, { ColourCode::Yellow_fg });
	AnimatePrint("world.toggleTelementary(false)", "", 3, { ColourCode::Yellow_fg });
	SimLoading("Apply World Settings", 10, 10);

	SlowPrint("Setup our new world", 40, 1, RENDER_GREEN);
	AnimatePrint("World world = new World(5);", "", 1, { ColourCode::Yellow_fg });
	AnimatePrint("world.addThing(me);", "", 1, { ColourCode::Yellow_fg });
	AnimatePrint("world.addThing(you);", "", 1, { ColourCode::Yellow_fg });
	SlowPrint("Let's begin...", 30, 1, RENDER_GREEN);
	SlowPrint("T H E  S I M U L A T I O N   ", 45, 1, RENDER_GREEN);
	ClearTerminal();
	AnimatePrint("world.ActivateSimulation();", "", 5, { ColourCode::Yellow_fg });
	SimWorld();
	PrintFromFile("assets/getServer.txt");
	std::this_thread::sleep_for(std::chrono::milliseconds(180));
#pragma endregion

#pragma region First_Verse
#ifdef _DEBUG
	Verse1:
	log_timestamp(SongSection::Verse1, audio->GetTime());
#endif
	PrintFromFile("assets/newWorldSettings.txt");

	SlowPrint("If I'm a set of points.", 50, 1, RENDER_GREEN);
	SlowPrint("Then I will give you my dimensions", 45, 0, RENDER_GREEN);
	AnimatePrint("getDimensions();", "[Console] Then I will give you my ", 5, RENDER_HIGHLIGHT, RENDER_GREEN);
	PrintFromFile("assets/getDimension.txt");

	SlowPrint("If I'm a circle.", 70, 1, RENDER_GREEN);
	SlowPrint("Then I will give you my circumference.", 45, 0, RENDER_GREEN);
	AnimatePrint("getCircumference();", "[Console] Then I will give you my ", 5, RENDER_HIGHLIGHT, RENDER_GREEN);
	PrintFromFile("assets/getCircumference.txt");

	SlowPrint("If I'm a sineWave.", 50, 1, RENDER_GREEN);
	SlowPrint("Then you can sit on all my tangents", 50, 0, RENDER_GREEN);
	AnimatePrint("getTangets();", "[Console] Then you can sit on all my ", 5, RENDER_HIGHLIGHT, RENDER_GREEN);
	PrintFromFile("assets/getTangent.txt");

	SlowPrint("If I approach infinity", 70, 1, RENDER_GREEN);
	AnimatePrint("[Console] Then you can be my limitations", "", 10, RENDER_GREEN);
	ScrambleTextWall(1, 600, {
		" 22145432389",
		"    32312421424242",
		"     4124143545433",
		(padStart(std::string(""), 7, ' ') + GetBlocks(9)).c_str(),
		(padStart(std::string(""), 9, ' ') + GetBlocks(9)).c_str(),
		"you.appplyLimitation(me,int.max);"
		});
#pragma endregion

#pragma region Pre_Chorus
#ifdef _DEBUG
	PreChorus1:
	log_timestamp(SongSection::PreChorus1, audio->GetTime());
#endif
	SlowPrint("Switch my Currrent", 80, 1, RENDER_GREEN);
	PrintFromFile("assets/AC2DC.txt");
	ScrambleTextWall(1, 600, {
		"DC ---> AC",
		"Conve",
		"Converting..."
		"Converting.. to AC to DC",
		std::string(padStart(std::string(""), 36, ' ') + " 98%   ").c_str(),
		"               Converted AC to DC               "
		});

	SlowPrint("And then blind my Vision", 70, 1, RENDER_GREEN);
	SlowPrint("So dizzy,", 120, 1, RENDER_GREEN);
	AnimatePrint("WARNING: Setting me.toggleVision() OFF will -----", "", 5, { ColourCode::Yellow_fg });
	AnimatePrint("WARNING: Setting me.toggleVision() OFF will -----", "", 5, { ColourCode::Yellow_fg });
	SlowPrint("So dizzy,", 120, 1, RENDER_GREEN);
	AnimatePrint("world.disableWarnings()", "", 5, { ColourCode::Yellow_fg });

	SlowPrint("Oh, we can travel to ... ", 80, 1, RENDER_GREEN);
	SlowPrint("AD to BC", 120, 1, RENDER_GREEN);
	AnimatePrint("Year 2332 BC", "[World] Setting Date to: ", 5, { ColourCode::Yellow_fg });
	ScrambleTextWall(1, 300, {
		std::string(std::string("[World] Setting Locations:") + GetBlocks(12)).c_str(),
		std::string(std::string("[World]: Setting Locations: BABY") + GetBlocks(12)).c_str(),
		"[World]: Setting Locations: BABYLON, Year: 2332 B.C"
		});

	SlowPrint("And we can unite", 70, 1, RENDER_GREEN);
	PrintFromFile("assets/getDeeply.txt");
	SlowPrint("So deeply, ", 80, 1, RENDER_GREEN);
	AnimatePrint("me.addLover(\"you\")", "", 3, { ColourCode::Yellow_fg });
	SlowPrint("So deeply, ", 80, 1, RENDER_GREEN);
	AnimatePrint("me.addLover(\"me\")", "", 3, { ColourCode::Yellow_fg });
#pragma endregion

#pragma region Chrous
#ifdef _DEBUG
	Chorus1:
	log_timestamp(SongSection::Chorus1, audio->GetTime());
#endif
	SlowPrint("If I can", 110, 1, RENDER_GREEN);
	SlowPrint("If I can", 110, 1, RENDER_GREEN);
	SlowPrint("Give you all the", 60, 1, RENDER_GREEN);
	SlowPrint("S T I M U L A T I O N S", 60, 1, RENDER_GREEN);
	AnimatePrint("you.getAffections();", "[World] Apply ", 3);

	SlowPrint("Then I can, then I can be your only S A T I S F A C T I O N", 40, 1, RENDER_GREEN);
	AnimatePrint("world.removeCharacters(\"all\", \"nonEssential\");", "", 3, { ColourCode::Yellow_fg });
	AnimatePrint("[REDACTED]", "[World] Deleting Character: ", 2);
	AnimatePrint("[REDACTED]", "[World] Deleting Character: ", 2);

	SlowPrint("If I can make you happy,", 50, 1, RENDER_GREEN);
	SlowPrint("I will run the ", 50, 0,  RENDER_GREEN);
	SlowPrint("world.execution();", 50, 1, RENDER_RED);
	AnimatePrint("Simulation already active", "[World] ", 2);
	PrintFromFile("assets/newWorldSettings.txt");

	SlowPrint("Though we are trapped in this STRANGE, strange simulation", 50, 1, RENDER_GREEN);
#pragma endregion

#pragma region Second_Verse
#ifdef _DEBUG
	Verse2:
	log_timestamp(SongSection::Verse2, audio->GetTime());
#endif
	SlowPrint("If I'm an eggplant", 80, 1, RENDER_GREEN);
	PrintFromFile("assets/getEggplant.txt");
	SlowPrint("Then I will give you my nutrients", 60, 1, RENDER_GREEN);
	AnimatePrint("if (std::is_same_v<decltype(me), Eggplant>) { you.addAttribute(me.getAttribute(\"nutrients\")); }", "", 2);

	SlowPrint("If I'm a tomato", 75, 1, RENDER_GREEN);
	PrintFromFile("assets/getTomato.txt");
	SlowPrint("Then I will give you my antioxidants", 60, 1, RENDER_GREEN);
	AnimatePrint("if (std::is_same_v<decltype(me), Tomato>) { you.addAttributes(me.getAttributes(\"antioxidants\")); }", "", 2);

	SlowPrint("If I'm a tabby cat", 85, 1, RENDER_GREEN);
	PrintFromFile("assets/getCat.txt");
	SlowPrint("Then I will give purr for your enjoyment", 40, 1, RENDER_GREEN);
	AnimatePrint("if (std::is_same_v<decltype(me), Cat>) { you.addAttribute(me.getLanguage(\"meow\")); }", "", 2);

	SlowPrint("If I'm the only ", 40, 0, RENDER_GREEN);
	SlowPrint("GOD", 40, 1, RENDER_RED);
	for (int i = 0; i < 3; ++i) AnimatePrint("[REDACTED] as Owner", "[World] Set player ", 2);
	SlowPrint("Then you're the proof of my ", 60, 0, RENDER_GREEN);
	AnimatePrint("E X I S T A N C E ", "[World] Set player ", 3, RENDER_RED, RENDER_GREEN);
#pragma endregion

#pragma region Pre_Chorus
#ifdef _DEBUG
	PreChorus2:
	log_timestamp(SongSection::PreChorus2, audio->GetTime());
#endif
	SlowPrint("Switch my gender to ", 90, 0, RENDER_GREEN);
	SlowPrint("F to M", 90, 1, RENDER_RED);
	AnimatePrint("Male", "[World] Set player's gender: ", 3);

	SlowPrint("And then do whatever from ", 90, 0, RENDER_GREEN);
	SlowPrint("AM to PM", 90, 1, RENDER_RED);
	AnimatePrint("UTC + 10", "[World] Changing Timezone: ", 5);
	AnimatePrint(FormatTime(), "[World] Updating Time: ", 5);

	SlowPrint("Oh, switch my role to ", 95, 0, RENDER_GREEN);
	SlowPrint("S to M", 95, 1, RENDER_RED);
	SlowPrint("me.toggleLoveable();", 55, 1, { ColourCode::Yellow_fg });

	SlowPrint("So we can enter the ...", 80, 1, RENDER_GREEN);
	ScrambleTextWall(2, 1500, {
		"th",
		"the T",
		"the Tra",
		"the Tran%&^*(",
		"Y#3 Yi@mk3*:$9<",
		"      The Trance"
		}, { ColourCode::Cyan_fg });
#pragma endregion

#pragma region Chorus
#ifdef _DEBUG
	Chorus2:
	log_timestamp(SongSection::Chorus2, audio->GetTime());
#endif
	SlowPrint("If I can,", 80, 1, RENDER_GREEN);
	SlowPrint("If I can, feel your vibrations", 80, 1, RENDER_GREEN);
	PrintFromFile("assets/getVibration.txt");

	SlowPrint("Then I can,", 70, 1, RENDER_GREEN);
	SlowPrint("Then I can finally be completion", 70, 1, RENDER_GREEN);
	SimLoading("C O M P L E T I O N", 20, 10);

	ClearTerminal();
	SlowPrint("Though you have left,", 70, 1, RENDER_GREEN);
	SlowPrint("Though you have left,", 70, 1, RENDER_GREEN);
	for (int i = 0; i < 3; ++i) AnimatePrint("THOUGH YOU HAVE LEFT", "[Console] ", 10, RENDER_RED, RENDER_RED);

	SlowPrint("You have left me in isolation", 73, 1, RENDER_GREEN);
#pragma endregion

#pragma region Post_Chorus
#ifdef _DEBUG
	PostChorus:
	log_timestamp(SongSection::PostChorus, audio->GetTime());
#endif
	SlowPrint("If I can,", 80, 1, RENDER_GREEN);
	SlowPrint("If I can erase all the pointless fragments", 75, 1, RENDER_GREEN);

	SlowPrint("Then maybe,", 80, 1, RENDER_GREEN);
	SlowPrint("Then maybe, you won't leave me so disheartened", 55, 1, RENDER_GREEN);

	SlowPrint("Challenging your ", 100, 0, RENDER_GREEN);
	SlowPrint("God", 100, 1, RENDER_RED);
	AnimatePrint("$@$%)#()#*)#*^^#(", "[Console] ", 9, RENDER_RED, RENDER_RED);

	SlowPrint("You have made some ILLEGAL ARGUMENTS *", 94, 1, RENDER_RED);
	SlowPrint("(*&(%#)()_%(# some ILLEGAL ARGUMENTS *", 92, 1, RENDER_RED);
	SlowPrint("You ha40)(*$)_@%@$ ILLEGAL ARGUMENTS *", 92, 1, RENDER_RED);
	SlowPrint("You have 78^*&(*)*$#LLEGAL ARGUMENTS *", 92, 1, RENDER_RED);
	SlowPrint("You have made some ILLEGAL ARGUMENTS *", 92, 1, RENDER_RED);

	PrintFromFile("assets/newWorldSettings.txt");
	PrintFromFile("assets/getError.txt");
	PrintFromFile("assets/getError2.txt");
#pragma endregion

#pragma region Bridge
#ifdef _DEBUG
	Bridge:
	log_timestamp(SongSection::Bridge, audio->GetTime());
#endif
	ClearTerminal();
	for (int i = 0; i < 12; ++i) SlowPrint("EXECUTION", 93, 1, RENDER_RED);
	ClearTerminal();

	SlowPrint("world.translate('1', \"de\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Ein                        \n");
	atLineStart = true;

	SlowPrint("world.translate('2', \"es\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Dos                        \n");
	atLineStart = true;

	SlowPrint("world.translate('3', \"fr\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Trois                      \n");
	atLineStart = true;

	SlowPrint("world.translate('4', \"ko\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Ne                         \n");
	atLineStart = true;

	SlowPrint("world.translate('5', \"sv\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Fem                        \n");
	atLineStart = true;

	SlowPrint("world.translate('6', \"zh\");", 13, 0, RENDER_RED);
	std::cout << RENDER_RED("\r[Console] Liu                        \n");
	atLineStart = true;

	SlowPrint("EXECUTION!", 70, 1, RENDER_RED);

	PrintFromFile("assets/getError.txt", RENDER_RED);
	PrintFromFile("assets/getError2.txt", RENDER_RED);
#pragma endregion

#pragma region Third_Chorus
#ifdef _DEBUG
	Chorus3:
	log_timestamp(SongSection::Chorus3, audio->GetTime());
#endif
	ClearTerminal();
	SlowPrint("If I can, if I can give them all the EXECUTION", 70, 1, RENDER_RED);
	SlowPrint("Then I can, then I can be your only EXECUTION", 70, 1, RENDER_RED);
	SlowPrint("If I can have you back, I will run the EXECUTION", 70, 1, RENDER_RED);

	PrintFromFile("assets/getError2.txt");;
	SlowPrint("Though we are trapped, we are trapped, ah ah ah ah", 60, 1, RENDER_RED);
#pragma endregion

#pragma region Fourth_Chrous_Refrain
#ifdef _DEBUG
	Chorus4:
	log_timestamp(SongSection::Chorus4, audio->GetTime());
#endif
	ClearTerminal();
	SlowPrint("I've studied, I've studied how to properly l-o-ove", 75, 1, RENDER_RED);
	SlowPrint("Question me, question me I can answer all l-o-ove", 70, 1, RENDER_RED);;
	PrintFromFile("assets/getError2.txt");
	ClearTerminal();

	SlowPrint("I know the algebraic expression of l-o-ove", 65, 1, RENDER_RED);
	SlowPrint("Though you are free, I am trapped, trapped in l-o-o-ove", 60, 1, RENDER_RED);

	SimWorld2();
	PrintFromFile("assets/getError2.txt");
#pragma endregion

#pragma region Outro
#ifdef _DEBUG
	Outro:
	log_timestamp(SongSection::Outro, audio->GetTime());
#endif
	ClearTerminal();
	SlowPrint("EXECUTION", 70, 1, RENDER_RED);

	for (int i = 0; i < 4; ++i) PrintFromFile("assets/getError2.txt");
#pragma endregion

#ifdef _DEBUG
	if (isSectionSkipped)
	{
		std::cout << Renderer{ ColourCode::Cyan_fg }("\nSkipped saving song positions.");
		std::cout << Renderer{ ColourCode::Cyan_fg }("\nSkipped saving used assets.");
		return;
	}

	save_song_positions();
	save_used_files();
#endif
}