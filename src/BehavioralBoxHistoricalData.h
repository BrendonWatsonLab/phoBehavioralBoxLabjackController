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

	void getHistoricalDataEvents();
	std::vector<unsigned long long> getFinalMillisecondsSinceEpoch() { return this->output_milliseconds_since_epoch; };
	std::vector<std::vector<double>> getFinalOutputValues() { return this->output_values; };


private:
	std::string boxID_;
	int labjackSerialNumber_;
	std::string dataFilesSearchDirectory_;

	// A list of data files for that particular box
	std::vector<LabjackDataFile> dataFiles_;

	// Accumulated line timestamps and values for all files:
	std::vector<unsigned long long> milliseconds_since_epoch;
	std::vector<std::vector<double>> values;

	// Parsed outputs
	std::vector<unsigned long long> output_milliseconds_since_epoch;
	std::vector<std::vector<double>> output_values;

	// Variable event outputs:
	// A vector (corresponding to an entry for each variable) of vectors (corresponding to an entry for each event for a given variable) of type pair.
	// The pair contains the timestamp and the new value at that timestamp
	//std::array<std::vector<pair<unsigned long long, double>>, NUM_CHANNELS> variableEventVectors;
	std::vector< std::vector< std::pair<unsigned long long, double> > > variableEventVectors;

	void findDataFiles();
	void reloadDataFiles();
	void sort() {
		std::sort(dataFiles_.begin(), dataFiles_.end());
	}

};

