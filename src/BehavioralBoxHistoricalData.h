#pragma once
#include <time.h>
#include <chrono>
#include <vector>

#include "LabjackDataFile.h"

typedef std::chrono::system_clock Clock;

//typedef std::tuple<unsigned long long, std::chrono::time_point<Clock>, double> ParsedVariableEventType;

// ParsedVariableEvent represents an event that was parsed from a historical CSV file
struct ParsedVariableEvent {
	unsigned long long milliseconds_since_epoch;
	std::chrono::time_point<Clock> datetime;
	double labjackValue;

	ParsedVariableEvent(unsigned long long milliseconds_since_epoch, std::chrono::time_point<Clock> datetime, double labjackValue) : milliseconds_since_epoch(milliseconds_since_epoch), datetime(datetime), labjackValue(labjackValue)
	{

	}

	// Can be called with std::sort(vec.begin(), vec.end(),greater<MyStruct>());
	bool operator > (const ParsedVariableEvent& event) const
	{
		return (milliseconds_since_epoch > event.milliseconds_since_epoch);
	}

	bool operator < (const ParsedVariableEvent& event) const
	{
		return (milliseconds_since_epoch < event.milliseconds_since_epoch);
	}

	// Tries to get the day of the month for the purpose of clustering events by day.
	tm getDateDay() {
		time_t tt = Clock::to_time_t(this->datetime);
		tm local_tm = *localtime(&tt);
		//std::cout << local_tm.tm_year + 1900 << '\n';
		//std::cout << local_tm.tm_mon + 1 << '\n';
		//std::cout << local_tm.tm_mday << '\n';
		local_tm.tm_hour = 0;
		local_tm.tm_min = 0;
		local_tm.tm_sec = 0;
		return local_tm;
	}

};

typedef ParsedVariableEvent ParsedVariableEventType;


// Serves to keep track of the historical data for a specific Behavioral Box. 
// Most specifically the record of Labjack events that have been loaded/parsed from the the .CSV output files
class BehavioralBoxHistoricalData
{
public:

	BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::string boxID, std::vector<LabjackDataFile> dataFiles);
	BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::vector<LabjackDataFile> dataFiles);

	void getHistoricalDataEvents();
	std::vector<unsigned long long> getFinalMillisecondsSinceEpoch() { return this->output_milliseconds_since_epoch; };
	std::vector<std::vector<double>> getFinalOutputValues() { return this->output_values; };

	int getNumberVariables() { return this->variableEventVectors.size(); };
	std::vector< ParsedVariableEventType > getEvents(int forVariableIndex) { return this->variableEventVectors[forVariableIndex]; };

	// returns the number of events in each variable
	std::vector<int> getNumberEvents();
	// get the maximum number of events any variable has
	int getMaxNumberEvents();
	// Returns a vector of the header strings that were parsed from the files
	std::vector<std::string> getHeaderLabels() { return this->headerLabels_; };



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
	std::vector<std::string> headerLabels_;

	// Variable event outputs:
	// A vector (corresponding to an entry for each variable) of vectors (corresponding to an entry for each event for a given variable) of type pair.
	// The pair contains the timestamp and the new value at that timestamp
	//std::array<std::vector<pair<unsigned long long, double>>, NUM_CHANNELS> variableEventVectors;
	std::vector< std::vector< ParsedVariableEventType > > variableEventVectors;

	void reloadDataFilesContents();
	void sort() {
		std::sort(dataFiles_.begin(), dataFiles_.end());
	}

};

