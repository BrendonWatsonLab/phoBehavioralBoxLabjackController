#pragma once
//#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "LoadedConfiguration.h"
#include "External/INIReader/INIReader.h"




class ConfigurationFile
{
public:
	ConfigurationFile();
	ConfigurationFile(std::string filePath);

	bool reloadFromFile();
	bool saveToFile();
	bool saveToFile(std::string overrideFilepath);

	LoadedConfiguration getLoadedConfig() { return this->loadedConfig; }

	enum class ConfigFileLoadStatus { LoadedFromFile, ExistsOnlyInMemory };

	

private:
	LoadedConfiguration loadedConfig;
	std::string filePath;
	INIReader iniReader;

	ConfigFileLoadStatus loadStatus;

	//bool isLoadedFromFile = false;
};


//void to_json(json& j, const ConfigurationFile& p) {
//	j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
//}
//
//void from_json(const json& j, ConfigurationFile& p) {
//	j.at("name").get_to(p.name);
//	j.at("address").get_to(p.address);
//	j.at("age").get_to(p.age);
//}
