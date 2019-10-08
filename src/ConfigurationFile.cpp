#include "ConfigurationFile.h"
#include <fstream>
#include <string>
#include <iostream>

//using json = nlohmann::json;

ConfigurationFile::ConfigurationFile(): ConfigurationFile("DefaultConfig.ini")
{
}

ConfigurationFile::ConfigurationFile(std::string filePath): iniReader(filePath)
{
	this->filePath = filePath;
	this->reloadFromFile();
}



bool ConfigurationFile::reloadFromFile()
{
	if (this->iniReader.ParseError() != 0) {
		std::cout << "Can't load 'test.ini'\n";
		return false;
	}

	this->loadedConfig.daylightStartHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightStartHour", 6);
	this->loadedConfig.daylightOffHour = this->iniReader.GetInteger("DEFAULT", "globalDaylightOffHour", 18);

	this->loadedConfig.launch_web_server = this->iniReader.GetBoolean("WebServer", "LAUNCH_WEB_SERVER", true);

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

