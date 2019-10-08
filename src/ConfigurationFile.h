#pragma once
//#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "LoadedConfiguration.h"
#include "External/INIReader.h"



class ConfigurationFile
{
public:
	ConfigurationFile();
	ConfigurationFile(std::string filePath);

	INIReader iniReader;

	bool reloadFromFile();
	bool saveToFile();
	bool saveToFile(std::string overrideFilename);

	std::string filePath;

	LoadedConfiguration getLoadedConfig() { return this->loadedConfig; }

private:
	LoadedConfiguration loadedConfig;
};

