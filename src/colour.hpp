#pragma once

#include <string>

enum class ColourCode : char	// ANSI escape codes
{
	Red_fg		=	31,
	Green_fg	=	32,
	Blue_fg		=	34,
	Yellow_fg	=	33,
	Cyan_fg		=	36,
	Magenta_fg	=	35,
	Default_fg	=	37,	// white
	Black_fg	=	30,

	Red_bg		=	41,
	Green_bg	=	42,
	Blue_bg		=	44,
	Yellow_bg	=	43,
	Cyan_bg		=	46,
	Magenta_bg	=	45,
	White_bg	=	47,
	Default_bg	=	40,	// black
};

struct Colour
{
	ColourCode		m_fg		= ColourCode::Default_fg;
	ColourCode		m_bg		= ColourCode::Default_bg;
	bool			m_dark		= false;
	bool			m_underline	= false;
	

	std::string		operator()() const;
};