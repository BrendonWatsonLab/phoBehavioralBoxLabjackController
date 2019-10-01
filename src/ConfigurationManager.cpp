#include "ConfigurationManager.h"
#include <iostream>
#include <cstdlib>

void ConfigurationManager::getEnvironmentVariables()
{
	if (const char* env_p = std::getenv("PATH")) {
		std::cout << "Your PATH is: " << env_p << '\n';
	}
}

void ConfigurationManager::setEnvironmentVariables()
{
}
