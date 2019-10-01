#pragma once

#include <string>
#include <regex>

static const std::regex behavioral_box_computer_hostname_regex("WATSON-BB-(\\d{2})");

class WindowsHelpers
{
public:
	WindowsHelpers();

	static std::string getHostName();
	//static std::string getExePath();
};

