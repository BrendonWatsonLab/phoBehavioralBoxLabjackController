#include "ConfigurationManager.h"
#include <iostream>
#include <cstdlib>
#include <fstream> // used to write out JSON data
#include "WindowsHelpers.h"
#include <nlohmann/json.hpp>

// for convenience
//using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;


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
		std::cout << "Couldn't parse number from " << hostname << ". It's not of the expected format \"WATSON-BB-XX\"." << std::endl;
		return -1;
	}
	std::string numbersMatchString = stringMatch[1];
	int numberOutResult = std::stoi(numbersMatchString);
	return numberOutResult;
}

bool ConfigurationManager::reloadFromConfigFile()
{
	return this->configFile.reloadFromFile();
}


std::string ConfigurationManager::getGeneratedActiveOutputDirectory()
{
	std::string outputPath = this->configFile.getLoadedConfig().outputFileRootDirectory;
	outputPath += this->configFile.getLoadedConfig().experimentName + "/";
	outputPath += this->configFile.getLoadedConfig().cohortName + "/";
	outputPath += this->configFile.getLoadedConfig().animalName + "/";
	return outputPath;
}

std::string ConfigurationManager::getGeneratedActiveHistoricalSearchDirectory()
{
	if (!this->configFile.getLoadedConfig().enableOverrideDefaultHistoricalFileSearchDirectory) {
		return this->getGeneratedActiveOutputDirectory(); // Return the standard output dir as the historical dir (default case)
	}
	else {
		return this->configFile.getLoadedConfig().historicalFileSearchRootDirectory;
	}
}

bool ConfigurationManager::saveConfig()
{
	return this->configFile.saveToFile();
}

void ConfigurationManager::testJsonConfig()
{
	

	// Test Config:
	std::cout << "ConfigurationManager::testJsonConfig() - Building test config objects..." << std::endl;
	//"AIN0"
	auto ChannelNames = std::vector<std::string>({ "AIN0","AIN1","AIN2","AIN3","SIGNALS_Dispense","Stream_Offset_Timer" }); // FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
	auto PortNames = std::vector<std::vector<std::string>>({ {"AIN0"},{"AIN1"},{"AIN2"},{"AIN3"},{"FIO_STATE"},{"SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16" } }); // FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
	auto PortPurpose = std::vector<std::vector<std::string>>({ {"Water1_BeamBreak"},{"Water2_BeamBreak"},{"Food1_BeamBreak"},{"Food2_BeamBreak"},{"SIGNALS_Dispense"},{"SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"} });

	auto newConfigSetup = LoadedLogicalChannelsSetupConfiguration();
	
	for (int i = 0; i < ChannelNames.size(); ++i)
	{
		auto currChannelConfig = LoadedLogicalChannelConfiguration();
		currChannelConfig.name = ChannelNames[i];
		currChannelConfig.portNames = PortNames[i];
		currChannelConfig.portPurpose = PortPurpose[i];
		newConfigSetup.logicalChannelConfigs.push_back(currChannelConfig);
	}

	std::cout << "\t done. Building Json.... ";


	this->configFile.updateActiveChannelSetupConfig(newConfigSetup); // update the config file's internal config setup

	std::string desiredJsonSavePath = "C:/Common/config/phoBehavioralBoxLabjackController-LogicalChannelSetupConfig.json";
	std::cout << "Trying to save json out to " << desiredJsonSavePath << "...";
	bool wasSaveSuccess = this->configFile.saveChannelConfigToFile(desiredJsonSavePath);
	if (!wasSaveSuccess)
	{
		// save failed
		std::cout << " !! Saving failed! Sorry!" << std::endl;
		return;
	}
	else
	{
		// save successful
		std::cout << " Save success!" << std::endl;
	}

	
	

	newConfigSetup.buildLogicalInputChannels(); // can be called to get the actual values
	

	// conversion: LoadedLogicalChannelsSetupConfiguration -> json
	//ordered_json j;

	json j = newConfigSetup;
	std::cout << " done. \n" << j << std::endl << std::endl;
	
	//#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","SIGNALS_Dispense","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
	return;
}
