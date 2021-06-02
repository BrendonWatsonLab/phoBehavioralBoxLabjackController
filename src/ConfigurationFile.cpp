#include "ConfigurationFile.h"
#include <fstream>
#include <string>
#include <iostream>

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
		return false;
	}

	//DEFAULT:
	this->loadedConfig.daylightStartHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightStartHour", 6);
	this->loadedConfig.daylightOffHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightOffHour", 18);
	this->loadedConfig.continue_without_labjacks = this->iniReader.GetBoolean("DEFAULT", "CONTINUE_WITHOUT_LABJACKS", true);
	
	
	this->loadedConfig.experimentName = this->iniReader.Get("DEFAULT", "experimentName", "experiment_00");
	this->loadedConfig.cohortName = this->iniReader.Get("DEFAULT", "cohortName", "cohort_00");
	this->loadedConfig.animalName = this->iniReader.Get("DEFAULT", "animalName", "animal_00");

	//Features:
	this->loadedConfig.enableHistoricalDataLoading = this->iniReader.GetBoolean("Features", "enableHistoricalDataLoading", false);
	this->loadedConfig.launch_web_server = this->iniReader.GetBoolean("Features", "LAUNCH_WEB_SERVER", false);

	//Filesystem:
	this->loadedConfig.outputFileRootDirectory = this->iniReader.Get("Filesystem", "globalOutputFileDirectory", "C:/Common/data/");
	this->loadedConfig.enableOverrideDefaultHistoricalFileSearchDirectory = this->iniReader.GetBoolean("Filesystem", "globalEnableOverrideDefaultHistoricalFileSearchDirectory", false);
	this->loadedConfig.historicalFileSearchRootDirectory = this->iniReader.Get("Filesystem", "globalHistoricalFileSearchDirectory", "C:/Common/data/");

	//TimeSeriesChart:
	this->loadedConfig.shouldEnableSynchronize_Y_Axis = this->iniReader.GetBoolean("TimeSeriesChart", "shouldEnableSynchronize_Y_Axis", true);
	this->loadedConfig.numDaysToDisplay = this->iniReader.GetInteger("TimeSeriesChart", "numDaysToDisplay", 60);

	return true;
}

bool ConfigurationFile::saveToFile()
{
	return this->saveToFile(this->filePath);
}

bool ConfigurationFile::saveToFile(std::string overrideFilename)
{
	if (overrideFilename != this->filePath) {
		this->filePath = overrideFilename;
	}

	//// write prettified JSON to another file
	//std::ofstream outputStream(this->filePath);
	////outputStream << std::setw(4) << this->serializedConfigJsonData << std::endl;
	//nlohmann::json j = this->serializedConfigJsonData;
	//outputStream << std::setw(4) << &j << std::endl;
	return false;
}

