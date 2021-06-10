#pragma once
#include <string>
#include "ConfigurationFile.h"

/* Serves to hold the program runtime configuration. Singleton.
Holds the settings such as the Labjack port config, timings, etc.
*/
class ConfigurationManager
{
public:
	
	// Get the environmental variables or something to determine the path to save the output CSVs to. Ideally this wouldn't be the archetecture (x86 or x64) and build-type (debug or release) specific binaries folder's "output_data" folder, and instead a common folder.
	void getEnvironmentVariables();
	void setEnvironmentVariables();

	std::string getHostName();
	int getNumericComputerIdentifier(); // Gets the 2-digit integer identifier for the current computer (and box, if there is a 1-to-1 mapping). Like the "02" in "WATSON-BB-02"

	LoadedConfiguration getLoadedConfig() { return this->configFile.getLoadedConfig(); }
	bool reloadFromConfigFile();

	// getGeneratedActiveOutputDirectory(): Returns an output path of the form "basePath/experimentName/cohortName/animalName" as requested by Brendon.
	std::string getGeneratedActiveOutputDirectory();
	std::string getGeneratedActiveHistoricalSearchDirectory();

	bool saveConfig();


	// JSON Channel Setup Config
	LoadedLogicalChannelsSetupConfiguration getLoadedChannelSetupConfig() const { return this->configFile.getLoadedChannelSetupConfig(); }
	bool tryLoadChannelConfigFromFile(std::string path);
	bool saveChannelConfigToFile(std::string path);

	
	void testJsonConfig();

private:
	std::string output_path_ = "";
	ConfigurationFile configFile;

	static LoadedLogicalChannelsSetupConfiguration buildSampleChannelSetupConfig();
};

