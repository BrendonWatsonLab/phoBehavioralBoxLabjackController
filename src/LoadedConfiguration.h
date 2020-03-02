#pragma once

struct LoadedConfiguration
{
	//DEFAULT:
	int daylightStartHour = 6;
	int daylightOffHour = 18;
	bool continue_without_labjacks = true;
	std::string experimentName = "experiment_00";
	std::string cohortName = "cohort_00";
	std::string animalName = "animal_00";

	//Filesystem:
	std::string outputFileRootDirectory = "C:/Common/data/";
	bool enableOverrideDefaultHistoricalFileSearchDirectory = false;
	std::string historicalFileSearchRootDirectory = "C:/Common/data/";

	//Features:
	bool launch_web_server = false;
	bool enableHistoricalDataLoading = false;

	//TimeSeriesChart:
	bool shouldEnableSynchronize_Y_Axis = false;
	int numDaysToDisplay = 60;
};