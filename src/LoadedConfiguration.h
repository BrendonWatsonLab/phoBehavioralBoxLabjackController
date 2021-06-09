#pragma once

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
	
	bool persistToIniFile(std::string path)
	{
		// Create Sections:
		//"DEFAULT"
		
	}
};