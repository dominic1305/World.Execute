#include "colour.hpp"

std::string Colour::operator()() const
{
	return std::to_string((int)m_fg) +		// Foreground colour
		";" +
		std::to_string((int)m_bg) +			// Background colour
		";" +
		(char)(49 + m_dark) +				// Dark? (49 "1" = normal, 50 "2" = dark)
		";" +
		(char)(m_underline * 3 + 49);		// Underline? (49 "1" = no underline, 52 "4" = underline)
}