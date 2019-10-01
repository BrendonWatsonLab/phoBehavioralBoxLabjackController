#pragma once
#include <string>

/* Serves to hold the program runtime configuration. Singleton.
Holds the settings such as the Labjack port config, timings, etc.
*/
class ConfigurationManager
{
public:
	
	// Get the environmental variables or something to determine the path to save the output CSVs to. Ideally this wouldn't be the archetecture (x86 or x64) and build-type (debug or release) specific binaries folder's "output_data" folder, and instead a common folder.
	void getEnvironmentVariables();
	void setEnvironmentVariables();

private:
	std::string output_path_ = "";
};

