#include "ConfigurationManager.h"
#include <iostream>
#include <cstdlib>
#include "WindowsHelpers.h"

void ConfigurationManager::getEnvironmentVariables()
{
	if (const char* env_p = std::getenv("PATH")) {
		std::cout << "Your PATH is: " << env_p << '\n';
	}
}

void ConfigurationManager::setEnvironmentVariables()
{
}

std::string ConfigurationManager::getHostName()
{
	return WindowsHelpers::getHostName();
}

// Gets the 2-digit integer identifier for the current computer (and box, if there is a 1-to-1 mapping). Like the "02" in "WATSON-BB-02"
int ConfigurationManager::getNumericComputerIdentifier()
{
	std::string hostname = ConfigurationManager::getHostName();
	if (hostname == "") { 
		std::cout << "Hostname empty!" << std::endl;
		return -1; 
	}
	std::smatch stringMatch;    // same as std::match_results<string::const_iterator> sm;
	std::regex_match(hostname, stringMatch, behavioral_box_computer_hostname_regex);
	if (stringMatch.size() <= 1) { 
		std::cout << "Couldn't parse number from " << hostname << std::endl;
		return -1; 
	}
	std::string numbersMatchString = stringMatch[1];
	int numberOutResult = std::stoi(numbersMatchString);
	return numberOutResult;
}
