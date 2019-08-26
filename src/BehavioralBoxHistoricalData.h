#pragma once
#include <vector>

#include "LabjackDataFile.h"

// Serves to keep track of the historical data for a specific Behavioral Box. 
// Most specifically the record of Labjack events that have been loaded/parsed from the the .CSV output files
class BehavioralBoxHistoricalData
{
public:

	BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::string boxID);
	BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber);

	// Find the Labjack data files for the labjack with the specified serial number
	static std::vector<LabjackDataFile> findDataFiles(std::string searchDirectory, int labjackSerialNumber);
	// NOTE: the start/end milliseconds are non-inclusive
	static std::vector<LabjackDataFile> findDataFiles(std::string searchDirectory, int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch);

	static void concatenateCsvFiles(std::vector<LabjackDataFile> dataFiles_);

private:
	std::string boxID_;
	int labjackSerialNumber_;
	std::string dataFilesSearchDirectory_;

	// A list of data files for that particular box
	std::vector<LabjackDataFile> dataFiles_;

	void findDataFiles();
	void reloadDataFiles();

};

