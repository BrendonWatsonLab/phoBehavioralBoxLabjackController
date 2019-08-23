#pragma once
#include <vector>

#include "LabjackDataFile.h"

// Serves to keep track of the historical data for a specific Behavioral Box. 
// Most specifically the record of Labjack events that have been loaded/parsed from the the .CSV output files
class BehavioralBoxHistoricalData
{
	std::string boxID = "";
	int labjackSerialNumber;
	// A list of data files for that particular box
	std::vector<LabjackDataFile> dataFiles;

	BehavioralBoxHistoricalData(int labjackSerialNumber);

	// Find the Labjack data files for the labjack with the specified serial number
	static std::vector<LabjackDataFile> findDataFiles(std::string searchDirectory, int labjackSerialNumber);
	// NOTE: the start/end milliseconds are non-inclusive
	static std::vector<LabjackDataFile> findDataFiles(std::string searchDirectory, int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch);

};

