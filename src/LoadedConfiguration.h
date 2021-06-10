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

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedConfiguration, config_version, daylightStartHour, daylightOffHour, continue_without_labjacks, experimentName, cohortName, animalName, outputFileRootDirectory,
		enableOverrideDefaultHistoricalFileSearchDirectory, historicalFileSearchRootDirectory, launch_web_server, enableHistoricalDataLoading, shouldEnableSynchronize_Y_Axis, numDaysToDisplay)

	//bool persistToIniFile(std::string path);
	
};


//
// The serializable seting for a single logical input channel
struct LoadedLogicalChannelConfiguration
{
	//enum class ChannelValueMode { CVM_Custom, AnalogAsDigitalInput, DigitalStateAsDigitalValues, AnalogAsContinuousInput, TimerRegistersAsContinuousTimer };
	enum ChannelValueMode { CVM_Custom, CVM_AnalogAsDigitalInput, CVM_DigitalStateAsDigitalValues, CVM_AnalogAsContinuousInput, CVM_TimerRegistersAsContinuousTimer };
	
	std::string name;
	std::vector<std::string> portNames;

	// Optional Properties:
	std::vector<std::string> portPurpose;
	ChannelValueMode channelValuesMode{ CVM_AnalogAsDigitalInput };
	
	// map ChannelValueMode values to JSON as strings
	//NLOHMANN_JSON_SERIALIZE_ENUM(ChannelValueMode, {
	//	{ChannelValueMode::Custom, nullptr},
	//	{ChannelValueMode::AnalogAsDigitalInput, "AnalogAsDigitalInput"},
	//	{ChannelValueMode::DigitalStateAsDigitalValues, "DigitalStateAsDigitalValues"},
	//	{ChannelValueMode::AnalogAsContinuousInput, "AnalogAsContinuousInput"},
	//	{ChannelValueMode::TimerRegistersAsContinuousTimer, "TimerRegistersAsContinuousTimer"},
	//})

	//NLOHMANN_JSON_SERIALIZE_ENUM(ChannelValueMode, {
	//	{CVM_Custom, "CUSTOM"},
	//	{CVM_AnalogAsDigitalInput, "AnalogAsDigitalInput"},
	//	{CVM_DigitalStateAsDigitalValues, "DigitalStateAsDigitalValues"},
	//	{CVM_AnalogAsContinuousInput, "AnalogAsContinuousInput"},
	//	{CVM_TimerRegistersAsContinuousTimer, "TimerRegistersAsContinuousTimer"},
	//})


	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedLogicalChannelConfiguration, name, portNames, portPurpose, channelValuesMode)
};


//
// The object that holds the vector of LoadedLogicalChannelConfiguration
struct LoadedLogicalChannelsSetupConfiguration
{
	std::vector<LoadedLogicalChannelConfiguration> logicalChannelConfigs;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(LoadedLogicalChannelsSetupConfiguration, logicalChannelConfigs)

	std::vector<LabjackLogicalInputChannel*> buildLogicalInputChannels();
	
};
