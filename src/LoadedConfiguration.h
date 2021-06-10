#pragma once
#include <nlohmann/json.hpp>

class LabjackLogicalInputChannel;
// for convenience
using json = nlohmann::json;
//using ordered_json = nlohmann::ordered_json;


struct LoadedConfiguration
{
	int config_version = 2;
	
	//DEFAULT:
	int daylightStartHour = 6;
	int daylightOffHour = 18;
	bool continue_without_labjacks = true;
	std::string experimentName = "experiment_00";
	std::string cohortName = "cohort_00";
	std::string animalName = "animal_00";

	//Filesystem:
	std::string outputFileRootDirectory = "C:/Common/data/";
	bool enableOverrideDefaultHistoricalFileSearchDirectory = true;
	std::string historicalFileSearchRootDirectory = "C:/Common/data/";

	//Features:
	bool launch_web_server = false;
	bool enableHistoricalDataLoading = false;

	//TimeSeriesChart:
	bool shouldEnableSynchronize_Y_Axis = false;
	int numDaysToDisplay = 60;

	//LABJACK:
	std::string labjackInputChannelList = "{AIN0,AIN1,AIN2,AIN3,FIO_STATE,SYSTEM_TIMER_20HZ,STREAM_DATA_CAPTURE_16}";


	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedConfiguration, config_version, daylightStartHour, daylightOffHour)


	bool persistToIniFile(std::string path);
	
};



// The serializable seting for a single logical input channel
struct LoadedLogicalChannelConfiguration
{
	int config_version = 2;
	std::string name;
	std::vector<std::string> portNames;

	// Optional Properties:
	std::vector<std::string> portPurpose;

	enum class ChannelValueMode { Custom, AnalogAsDigitalInput, DigitalStateAsDigitalValues, AnalogAsContinuousInput, TimerRegistersAsContinuousTimer };

	// map TaskState values to JSON as strings
	NLOHMANN_JSON_SERIALIZE_ENUM(ChannelValueMode, {
		{ChannelValueMode::Custom, nullptr},
		{ChannelValueMode::AnalogAsDigitalInput, "AnalogAsDigitalInput"},
		{ChannelValueMode::DigitalStateAsDigitalValues, "DigitalStateAsDigitalValues"},
		{ChannelValueMode::AnalogAsContinuousInput, "AnalogAsContinuousInput"},
	})

	ChannelValueMode channelValuesMode;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedLogicalChannelConfiguration, config_version, name, portNames, portPurpose, channelValuesMode)
};

// The object that holds the vector of LoadedLogicalChannelConfiguration
struct LoadedLogicalChannelsSetupConfiguration
{
	int config_version = 2;
	std::vector<LoadedLogicalChannelConfiguration> logicalChannelConfigs;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedLogicalChannelsSetupConfiguration, config_version, logicalChannelConfigs)

	std::vector<LabjackLogicalInputChannel*> buildLogicalInputChannels();
	
};
