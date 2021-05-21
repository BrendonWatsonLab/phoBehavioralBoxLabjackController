#include <limits>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "BehavioralBoxHistoricalData.h"
#include "FilesystemHelpers.h"
#include "LabjackHelpers.h"

// For exporting CSVs
#include "External/CSVWriter.h"



BehavioralBoxHistoricalData::BehavioralBoxHistoricalData()
{
}

BehavioralBoxHistoricalData::BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::string boxID, std::vector<LabjackDataFile> dataFiles): labjackSerialNumber_(labjackSerialNumber), dataFilesSearchDirectory_(searchDirectory), boxID_(boxID), dataFiles_(dataFiles)
{
	// Sort the data files by decending recency:
	this->sort();

	// TODO: load the data files in the background
	this->reloadDataFilesContents();

	// TODO: collect the events in the background
	this->getHistoricalDataEvents();
}

BehavioralBoxHistoricalData::BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::vector<LabjackDataFile> dataFiles) : BehavioralBoxHistoricalData(searchDirectory, labjackSerialNumber, std::to_string(labjackSerialNumber), dataFiles)
{

}



std::vector<int> BehavioralBoxHistoricalData::getNumberEvents()
{
	std::vector<int> outputCounts;
	for each (auto aVariableVect in this->variableEventVectors)
	{
		outputCounts.push_back(aVariableVect.size());
	}
	return outputCounts;
}

int BehavioralBoxHistoricalData::getMaxNumberEvents()
{
	std::vector<int> counts = this->getNumberEvents();
	int max_count = *std::max_element(std::begin(counts), std::end(counts));
	return max_count;
}

const void BehavioralBoxHistoricalData::exportAsCSV(std::string path)
{
	//TODO: path currently ignored
	CSVWriter csvOutput = CSVWriter(",");
	// Write the header to the .csv file:
	std::vector<std::string> headerLabels = this->headerLabels_;
	csvOutput.newRow();
	for (int i = 0; i < headerLabels.size(); i++) {
		csvOutput << headerLabels[i];
	}
	//csvOutput.writeToFile(path, false);

	for (int j = 0; j < this->output_milliseconds_since_epoch.size(); j++) {
		csvOutput.newRow() << this->output_milliseconds_since_epoch[j];
		for (int i = 0; i < this->output_values[j].size(); i++) {
			csvOutput << this->output_values[j][i];
		}
	}
	
	// Close the open output file:
	csvOutput.writeToFile(path, true);
}


//reloadDataFilesContents(): called after dataFiles_ is set and sorted.
void BehavioralBoxHistoricalData::reloadDataFilesContents()
{
	for (int i = 0; i < this->dataFiles_.size(); i++)
	{
		this->dataFiles_[i].reloadContents();
	}
}

//getHistoricalDataEvents(): called after reloadDataFilesContents()
/*
Populates the this->milliseconds_since_epoch and this->values variables with the parsed results of the lines
this->output_values and this->output_milliseconds_since_epoch contain only the values corresponding to events.
*/
void BehavioralBoxHistoricalData::getHistoricalDataEvents()
{
	this->milliseconds_since_epoch.clear();
	this->dataFileTimestamps.clear();
	this->values.clear();
	this->output_milliseconds_since_epoch.clear();
	this->output_values.clear();
	this->variableEventVectors.clear();
	this->headerLabels_.clear();
	this->timestampToIndexMap_.clear();

	//TODO: Do something special for analog (continuous) values?
	bool areHeaderLabelsSameForAllFiles = true;
	int hard_coded_header_column_maximum = 9;

	int numVariables = 0;
	int maxNumVariables = -1;
	std::vector<std::string> headerLabels;
	std::string debugPrintDelimiter = ",";
	std::stringstream debugStringStream;

	// For each data file object:
	for (int i = 0; i < this->dataFiles_.size(); i++)
	{
		std::string currDataFileName = this->dataFiles_[i].fileName;
		// Get dataFileTime:
		auto currDataFileTimestamp = this->dataFiles_[i].getFileTimestamp();
		this->dataFileTimestamps.push_back(currDataFileTimestamp);
		
		std::vector<LabjackDataFileLine> tempLines = this->dataFiles_[i].getParsedLines();
		std::vector<std::string> fileHeaderLabels = this->dataFiles_[i].getParsedHeaderLabels();


		if (fileHeaderLabels.size() > hard_coded_header_column_maximum) {
			std::cout << "DEBUG: " << this->dataFiles_[i].fullPath << " exceeds maximum column count of " << hard_coded_header_column_maximum << ", it has " << fileHeaderLabels.size() << ". Skipping." << std::endl;
			continue;
		}

		// If it's not the first file found and we already have a set of header labels
		if (i > 0) {
			bool areHeaderLabelsSame = true;
			if (headerLabels.size() == fileHeaderLabels.size()) {
				areHeaderLabelsSame = std::equal(headerLabels.begin(), headerLabels.end(), fileHeaderLabels.begin());
			}
			else {
				// If they aren't the same size, they aren't the same
				areHeaderLabelsSame = false;
			}
			if (!areHeaderLabelsSame) {
				areHeaderLabelsSameForAllFiles = false;
				// Generate a string from the headers to print the debug string if they're different
				std::string fileHeadersDebugString = "";
				for each (std::string aFileHeaderLabel in fileHeaderLabels)
				{
					fileHeadersDebugString += aFileHeaderLabel;
					fileHeadersDebugString += ",";
				}
				std::string allHeadersDebugString = debugStringStream.str();
				for each (std::string aHeaderLabel in headerLabels)
				{
					allHeadersDebugString += aHeaderLabel;
					allHeadersDebugString += ",";
				}
				std::cout << "WARNING: dataFile[" << currDataFileName << "] has <" << fileHeadersDebugString << "> while the previous headers were <" << allHeadersDebugString << ">." << std::endl;
			}
		}
		else {
			// TODO: we may not want to do this!
			// Update the header labels
			headerLabels = fileHeaderLabels;
		}


		// For each line the returned set of lines for a given file:
		for each (LabjackDataFileLine aLineObject in tempLines)
		{
			// Check the variables for each line. They should be the same within a file, but they may differ across files.
			numVariables = aLineObject.values.size();
			if (numVariables != maxNumVariables) {
				if (i > 0) {
					// If it's not the first file/line found
					std::cout << "dataFile[" << currDataFileName << "] has " << std::to_string(numVariables) << " variables while previous files only had " << std::to_string(maxNumVariables) << ". Need to adjust all with fewer variables to max." << std::endl;
				}
				maxNumVariables = std::max(maxNumVariables, numVariables);
			}

			this->milliseconds_since_epoch.push_back(aLineObject.milliseconds_since_epoch);
			this->values.push_back(aLineObject.values);
		} // end for each line
	} // end for each file
	//TODO: perhaps have a "reloading" event

	// Build arrays to hold the state change events for each variable
	for (int j = 0; j < maxNumVariables; j++) {
		std::vector<ParsedVariableEventType> newVariableVect;
		this->variableEventVectors.push_back(newVariableVect);
	}

	// Compute Differences and such
	int numSamples = this->milliseconds_since_epoch.size();
	if (numSamples < 2) { return; }

	unsigned long long prev_time = this->milliseconds_since_epoch[0];
	std::vector<double> prev_values = this->values[0];

	unsigned long long curr_time;
	std::vector<double> curr_values;
	std::vector<double> temp_output_values;

	int uniqueLineTimestampIndex = 0;
	bool shouldIncludeCurrentLine = false;
	// Loop through all the samples (lines)
	for (int i = 0; i < numSamples; i++)
	{
		shouldIncludeCurrentLine = false;
		temp_output_values.clear();

		curr_time = this->milliseconds_since_epoch[i];
		curr_values = this->values[i];
		std::chrono::time_point<Clock> curr_timepoint = LabjackHelpers::date_from_milliseconds_since_epoch(curr_time);

		//unsigned long long curr_time_diff = curr_time - prev_time;

		//TODO: deal with the first sample. 
		std::vector<double> currDiffVect = LabjackHelpers::computeDelta(curr_values, prev_values);

		// EVENT DETECTION by falling_edge thresholding:
		// Loop through the variables
		for (int j = 0; j < maxNumVariables; j++) {
			//double currDiff = curr_values[j] - prev_values[j];
			double currDiff = currDiffVect[j];
			if (currDiff < -0.01) {
				// Include the value because it's a falling_edge (transition to negative values)
				shouldIncludeCurrentLine = true;
				temp_output_values.push_back(1.0);
				// Add the event timestamp and value to the appropriate vector
				this->variableEventVectors[j].push_back(ParsedVariableEvent(curr_time, curr_timepoint, curr_values[j]));

			}
			else {
				temp_output_values.push_back(0.0);
			}
		}

		// Output the line if wanted
		if (shouldIncludeCurrentLine) {
			this->output_milliseconds_since_epoch.push_back(curr_time);
			this->output_values.push_back(temp_output_values);
			this->timestampToIndexMap_[curr_timepoint] = uniqueLineTimestampIndex;
			uniqueLineTimestampIndex++;
		}

		prev_time = curr_time;
		prev_values = curr_values;
	} // end sample for loop

	// Set the header labels
	this->headerLabels_ = headerLabels;

	// Build the statistics objects
	this->eventStatistics_ = EventStatistics(this->variableEventVectors);
}
