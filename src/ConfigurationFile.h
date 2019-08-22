#pragma once
#include <nlohmann/json.hpp>
//#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

// for convenience
using json = nlohmann::json;

class ConfigurationFile
{
public:
	ConfigurationFile();
	ConfigurationFile(std::string filePath);

	// String Serialization
	std::string toString();
	bool fromString(std::string sourceString);

	bool reloadFromFile();
	bool saveToFile();
	bool saveToFile(std::string overrideFilename);

	std::string filePath;

private:
	std::string serializedConfigJsonData = "";
};

