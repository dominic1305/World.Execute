#pragma once

#include <string>
#include <array>

struct _hex
{
	std::array<uint8_t, 3>	hex;

	constexpr				_hex(uint32_t val)
		: hex({ static_cast<uint8_t>(val & 0xFF), static_cast<uint8_t>((val & 0xFF00) >> 8), static_cast<uint8_t>((val & 0xFF0000) >> 16) })
	{
	}

	constexpr				_hex(const _hex& other)
		: hex(other.hex)
	{
	}

	_hex&					operator=(const _hex& other);

	constexpr operator		uint32_t() const
	{
		return (hex[0]) | (hex[1] << 8) | (hex[2] << 16);
	}
};

union rgb
{
	struct // Little-Endian
	{
		uint8_t				blue;
		uint8_t				green;
		uint8_t				red;
	};
	struct
	{
		_hex				hex;
	};

	constexpr				rgb(uint32_t val)
		: hex(val)
	{
	}

	constexpr				rgb(const rgb& other)
		: hex(other.hex)
	{
	}

	rgb&					operator=(const rgb& other);

	constexpr bool			operator==(rgb other) const
	{
		return hex == other.hex;
	}
};

namespace Colours // Colour presets from "https://en.wikipedia.org/wiki/ANSI_escape_code#Colors"
{
	constexpr rgb			Black		= 0x0C0C0C;

	constexpr rgb			Red_DK		= 0xC50F1F;
	constexpr rgb			Green_DK	= 0x13A10E;
	constexpr rgb			Yellow_DK	= 0xC19C00;
	constexpr rgb			Blue_DK		= 0x0037DA;
	constexpr rgb			Magenta_DK	= 0x881798;
	constexpr rgb			Cyan_DK		= 0x3A96DD;
	constexpr rgb			White_DK	= 0xCCCCCC;

	constexpr rgb			Red_LT		= 0xE74856;
	constexpr rgb			Green_LT	= 0x16C60C;
	constexpr rgb			Yellow_LT	= 0xF9F1A5;
	constexpr rgb			Blue_LT		= 0x3B78FF;
	constexpr rgb			Magenta_LT	= 0xB40098;
	constexpr rgb			Cyan_LT		= 0x61D6D6;
	constexpr rgb			White_LT	= 0xF2F2F2;
}

struct Colour
{
	rgb						fg		= Colours::White_LT;
	rgb						bg		= Colours::Black;
	bool					underline	= false;

	std::string				operator()() const;
};