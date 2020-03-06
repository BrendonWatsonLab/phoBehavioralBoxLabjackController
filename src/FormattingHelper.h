#pragma once
#include <string>
#include <cstdio>


class FormattingHelper
{
public:
	FormattingHelper() {};

	// returns a two-digit fixed-width string with a leading-zero if needed. Inverse of parse_number_string(...).
	static std::string format_two_digit_string(int number) {
		char buffer[2];
		std::snprintf(buffer, sizeof(buffer), "%02d", number);
		return buffer;
	}
	
	// parses an integer value from a two-digit fixed-width string. Inverse of format_two_digit_string(...).
	static int parse_number_string(std::string number_string) {
		return std::stoi(number_string);
	}



};


