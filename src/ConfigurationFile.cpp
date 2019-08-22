#include "ConfigurationFile.h"
#include <fstream>
#include <string>
#include <iostream>

//using json = nlohmann::json;

ConfigurationFile::ConfigurationFile(): ConfigurationFile("DefaultConfig.json")
{
}

ConfigurationFile::ConfigurationFile(std::string filePath)
{
	this->filePath = filePath;
}

std::string ConfigurationFile::toString()
{
	//json configData = json::Parse(this->serializedConfigJsonData);
	//return configData->dump(4);
	return this->serializedConfigJsonData;
}

bool ConfigurationFile::fromString(std::string sourceString)
{
	this->serializedConfigJsonData = sourceString;
	//json newJson = json::parse(sourceString);
	//nlohmann::json newJson;
	//newJson  = nlohmann::json this->serializedConfigJsonData._json;
	//this->configData = json::parse(sourceString.c_str());
	//this->configData = &newJson;
	return true;
}

bool ConfigurationFile::reloadFromFile()
{
	// read a JSON file
	std::ifstream inputFileStream(this->filePath);
	inputFileStream >> this->serializedConfigJsonData;
	//nlohmann::json j;
	//inputFileStream >> j;
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

	// write prettified JSON to another file
	std::ofstream outputStream(this->filePath);
	//outputStream << std::setw(4) << this->serializedConfigJsonData << std::endl;
	nlohmann::json j = this->serializedConfigJsonData;
	outputStream << std::setw(4) << &j << std::endl;
	return false;
}
