#include "ConfigurationFile.h"
#include <fstream>
#include <string>
#include <iostream>
#include "FilesystemHelpers.h"

//using json = nlohmann::json;

ConfigurationFile::ConfigurationFile(): ConfigurationFile("C:/Common/config/phoBehavioralBoxLabjackController-Config.ini")
{
}

ConfigurationFile::ConfigurationFile(std::string filePath): filePath(filePath), iniReader(INIReader(filePath))
{
	this->reloadFromFile();
}



bool ConfigurationFile::reloadFromFile()
{
	int parseError = this->iniReader.ParseError();
	if (parseError != 0) {
		std::cout << "\t ERROR: Can't load '" << this->filePath << "'\n";
		this->loadStatus = ConfigFileLoadStatus::ExistsOnlyInMemory;
		//return false;
		this->iniReader.beginDynamicIniBuilding();
	}
	else
	{
		// Successfully loaded from extant INI file:
		this->loadStatus = ConfigFileLoadStatus::LoadedFromFile;
	}

	//DEFAULT:
	this->loadedConfig.config_version = this->iniReader.GetInteger("DEFAULT", "config_version", 2);

	this->loadedConfig.daylightStartHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightStartHour", 6);
	this->loadedConfig.daylightOffHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightOffHour", 18);
	this->loadedConfig.continue_without_labjacks = this->iniReader.GetBoolean("DEFAULT", "CONTINUE_WITHOUT_LABJACKS", true);
	
	
	this->loadedConfig.experimentName = this->iniReader.GetString("DEFAULT", "experimentName", "experiment_00");
	this->loadedConfig.cohortName = this->iniReader.GetString("DEFAULT", "cohortName", "cohort_00");
	this->loadedConfig.animalName = this->iniReader.GetString("DEFAULT", "animalName", "animal_00");

	//Features:
	this->loadedConfig.enableHistoricalDataLoading = this->iniReader.GetBoolean("Features", "enableHistoricalDataLoading", false);
	this->loadedConfig.launch_web_server = this->iniReader.GetBoolean("Features", "LAUNCH_WEB_SERVER", false);

	//Filesystem:
	this->loadedConfig.outputFileRootDirectory = this->iniReader.GetString("Filesystem", "globalOutputFileDirectory", "C:/Common/data/");
	this->loadedConfig.enableOverrideDefaultHistoricalFileSearchDirectory = this->iniReader.GetBoolean("Filesystem", "globalEnableOverrideDefaultHistoricalFileSearchDirectory", false);
	this->loadedConfig.historicalFileSearchRootDirectory = this->iniReader.GetString("Filesystem", "globalHistoricalFileSearchDirectory", "C:/Common/data/");

	//TimeSeriesChart:
	this->loadedConfig.shouldEnableSynchronize_Y_Axis = this->iniReader.GetBoolean("TimeSeriesChart", "shouldEnableSynchronize_Y_Axis", true);
	this->loadedConfig.numDaysToDisplay = this->iniReader.GetInteger("TimeSeriesChart", "numDaysToDisplay", 60);
	
	switch (this->loadStatus)
	{
	case ConfigFileLoadStatus::LoadedFromFile: break;
	case ConfigFileLoadStatus::ExistsOnlyInMemory: 
		// If it exists only in memory, create it
		this->iniReader.writeDynamicIni(this->filePath);
		break;
	default: ;
	}
	

	return true;
}

bool ConfigurationFile::saveToFile()
{
	switch (FilesystemHelpers::filesystemItemStatus(this->filePath))
	{
	case FilesystemHelpers::FilesystemItemStatus::NonExistant: break;
	case FilesystemHelpers::FilesystemItemStatus::File: break;
	case FilesystemHelpers::FilesystemItemStatus::Directory: break;
	case FilesystemHelpers::FilesystemItemStatus::OtherExtant: break;
	default: ;
	}
	
	
	
	if (this->filePath == "") {
		throw std::invalid_argument("received negative value");
	}
	
	return this->saveToFile(this->filePath);
}

bool ConfigurationFile::saveToFile(std::string overrideFilepath)
{
	if (overrideFilepath != this->filePath) {
		this->filePath = overrideFilepath;
	}

	this->iniReader = INIReader(overrideFilepath);
	

	//// write prettified JSON to another file
	//std::ofstream outputStream(this->filePath);
	////outputStream << std::setw(4) << this->serializedConfigJsonData << std::endl;
	//nlohmann::json j = this->serializedConfigJsonData;
	//outputStream << std::setw(4) << &j << std::endl;
	return false;
}

void ConfigurationFile::updateActiveChannelSetupConfig(LoadedLogicalChannelsSetupConfiguration updatedConfig)
{
	// Saves the loadedChannelSetupConfig to file
	this->loadedChannelSetupConfig = updatedConfig;
}

// Saves this->loadedChannelSetupConfig to the JSON file specified by the provided path
bool ConfigurationFile::saveChannelConfigToFile(std::string path)
{
	auto currChannelSetupConfig = this->loadedChannelSetupConfig;
	// Convert to JSON
	// conversion: LoadedLogicalChannelsSetupConfiguration -> json
	//ordered_json j;
	json j = currChannelSetupConfig;
	// Save out to JSON file:
	std::ofstream o(path);
	o << std::setw(4) << j << std::endl;
	o.close(); // close the file
	return true;
}

bool ConfigurationFile::tryLoadChannelConfigFromFile(std::string path)
{
	// Open input stream to file:
	std::ifstream inputStream(path);
	json j;
	try
	{
		j = json::parse(inputStream);
	}
	catch (json::parse_error& ex)
	{
		std::cerr << "parse error at byte " << ex.byte << std::endl;
		return false;
	}

	std::cout << "Successfully read json data from " << path << "! Attempting to parse as a LoadedLogicalChannelsSetupConfiguration object..." << std::endl;
	// conversion: json -> person
	auto loadedSetupConfig = j.get<LoadedLogicalChannelsSetupConfiguration>();
	// After getting the loaded object, set this member variable:
	this->loadedChannelSetupConfig = loadedSetupConfig;

	std::cout << "Successfully loaded " << path << " as JSON and updated this->loadedChannelSetupConfig!" << std::endl;
	inputStream.close();
	return true;
}

