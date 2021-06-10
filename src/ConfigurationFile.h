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
	enum class ConfigFileLoadStatus { LoadedFromFile, ExistsOnlyInMemory };

	
	ConfigurationFile();
	ConfigurationFile(std::string filePath);

	bool reloadFromFile();
	bool saveToFile();
	bool saveToFile(std::string overrideFilepath);
	
	LoadedConfiguration getLoadedConfig() const { return this->loadedConfig; }


	LoadedLogicalChannelsSetupConfiguration getLoadedChannelSetupConfig() const { return this->loadedChannelSetupConfig; }
	void updateActiveChannelSetupConfig(LoadedLogicalChannelsSetupConfiguration updatedConfig);
	bool saveChannelConfigToFile(std::string path);
	bool tryLoadChannelConfigFromFile(std::string path);
	

private:
	LoadedConfiguration loadedConfig;
	LoadedLogicalChannelsSetupConfiguration loadedChannelSetupConfig;
	
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
