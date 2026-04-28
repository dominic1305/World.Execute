#include "colour.hpp"

_hex& _hex::operator=(const _hex& other)
{
	if (this != &other)
	{
		hex = other.hex;
	}

	return *this;
}

rgb& rgb::operator=(const rgb& other)
{
	if (this != &other)
	{
		hex = other.hex;
	}

	return *this;
}

std::string Colour::operator()() const
{
	return std::string((underline) ? "4;" : "")
		+ "38;2;"
		+ std::to_string((int)fg.red)
		+ ";"
		+ std::to_string((int)fg.green)
		+ ";"
		+ std::to_string((int)fg.blue)
		+ ";"
		+ "48;2;"
		+ std::to_string((int)bg.red)
		+ ";"
		+ std::to_string((int)bg.green)
		+ ";"
		+ std::to_string((int)bg.blue);
}