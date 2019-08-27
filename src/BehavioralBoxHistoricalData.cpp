#include <limits>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "BehavioralBoxHistoricalData.h"
#include "FilesystemHelpers.h"
#include "LabjackHelpers.h"


BehavioralBoxHistoricalData::BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::string boxID)
{
	this->labjackSerialNumber_ = labjackSerialNumber;
	this->dataFilesSearchDirectory_ = searchDirectory;
	this->boxID_ = boxID;

	// TODO: find data files in the background
	this->findDataFiles();

	// TODO: load the data files in the background
	this->reloadDataFiles();

	// TODO: collect the events in the background
	this->getHistoricalDataEvents();
}

BehavioralBoxHistoricalData::BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber): BehavioralBoxHistoricalData(searchDirectory, labjackSerialNumber, std::to_string(labjackSerialNumber))
{

}

std::vector<LabjackDataFile> BehavioralBoxHistoricalData::findDataFiles(std::string searchDirectory, int labjackSerialNumber)
{
	return findDataFiles(searchDirectory, labjackSerialNumber, 0, std::numeric_limits<unsigned long long>::max());
}

std::vector<LabjackDataFile> BehavioralBoxHistoricalData::findDataFiles(std::string searchDirectory, int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch)
{
	// Looks for files of the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}.csv"
	// activeLabjackSerialNumber: used to hold the parsed LabjackSerial Number when the requested one is 0.
	int activeLabjackSerialNumber = labjackSerialNumber; // only not equal to labjackSerialNumber if labjackSerialNumber == 0.
	// Build Serial Number String:
	std::string expectedSerialNumberString = "s" + std::to_string(labjackSerialNumber);
	std::vector<LabjackDataFile> outputVector = std::vector<LabjackDataFile>();
	//fs::path dir = "output_data/";
	fs::path dir = searchDirectory;
	std::vector<fs::path> foundFiles = scan_csv_files(dir);
	// Vector of string to save tokens 
	std::vector<std::string> fileNameParts;
	std::string intermediate;
	// Loop through the found files
	for (int i = 0; i < foundFiles.size(); i++) {
		std::string currFileBasename = foundFiles[i].stem().string(); // Stem returns the filename without the extension.
		// stringstream class check1 
		std::stringstream currNameSplitterStream(currFileBasename);
		// Clear the name parts for the next file
		fileNameParts.clear();
		// Tokenizing w.r.t. underscore '_' 
		while (getline(currNameSplitterStream, intermediate, '_'))
		{
			fileNameParts.push_back(intermediate);
		}
		if (fileNameParts.size() != 4) {
			std::cout << "Warning: File " << currFileBasename << " does not match the expected format (out_file_s{ SERIAL_NUMBER }_{ MILLISECONDS_SINCE_EPOCH }). Skipping." << std::endl;
			continue;
		}

		if (fileNameParts[2].empty()) {
			// serial string portition is empty
			continue;
		}

		if (labjackSerialNumber == 0) {
			// The "0" serial number is handled as a wildcard, returning results for any labjack.
			//fileNameParts[2].first
			const std::string serialOnlyString(fileNameParts[2].begin() + 1, fileNameParts[2].end());
			// Try to parse the string to the labjack serial number
			try {
				activeLabjackSerialNumber = std::stoi(serialOnlyString);
			}
			catch (const std::invalid_argument& ia) {
				std::cout << ia.what() << std::endl, 1;
				continue;
			}
			catch (const std::out_of_range& oor) {
				std::cout << oor.what() << std::endl, 2;
				continue;
			}
		}
		else {
			// Check if the serial number matches the search serial number
			if (fileNameParts[2] != expectedSerialNumberString) {
				// Serial number doesn't match
				continue;
			}
		}

		// Parse the milliseconds component
		std::string millisecondsComponentString = fileNameParts[3];
		unsigned long long millisecondsComponent = 0;
		try {
			millisecondsComponent = std::stoull(millisecondsComponentString, NULL, 0);
		}
		catch (...) {
			// Couldn't parse it to a unsigned long long
			std::cout << "Warning: File " << currFileBasename << " could not parse the milliseconds component " << millisecondsComponentString << " to an unsigned long long. Skipping." << std::endl;
			continue;
		}
		// Check whether it's within the time range the user provided
		if ((millisecondsComponent < startMillisecondsSinceEpoch) || (millisecondsComponent > endMillisecondsSinceEpoch)) {
			// It's outisde the available range
			continue;
		}
		// It meets all criteria to be returned. Add it to the output vector
		LabjackDataFile currFile = LabjackDataFile(foundFiles[i], activeLabjackSerialNumber, millisecondsComponent);
		outputVector.push_back(currFile);
	}
	return outputVector;
}

void BehavioralBoxHistoricalData::concatenateCsvFiles(std::vector<LabjackDataFile> dataFiles_)
{
	//TODO:
}

//getHistoricalDataEvents(): called after reloadDataFiles()
/*
Populates the this->milliseconds_since_epoch and this->values variables with the parsed results of the lines
*/
void BehavioralBoxHistoricalData::getHistoricalDataEvents()
{
	this->milliseconds_since_epoch.clear();
	this->values.clear();
	this->output_milliseconds_since_epoch.clear();
	this->output_values.clear();
	this->variableEventVectors.clear();
	this->headerLabels_.clear();

	int numVariables = 0;
	int maxNumVariables = -1;
	std::vector<std::string> headerLabels;
	std::string debugPrintDelimiter = ",";
	std::stringstream debugStringStream;

	// For each data file object:
	for (int i = 0; i < this->dataFiles_.size(); i++)
	{
		std::vector<LabjackDataFileLine> tempLines = this->dataFiles_[i].getParsedLines();
		std::vector<std::string> fileHeaderLabels = this->dataFiles_[i].getParsedHeaderLabels();
		// If it's not the first file found and we already have a set of header labels
		if (i > 0) {
			bool areHeaderLabelsSame = std::equal(headerLabels.begin(), headerLabels.end(), fileHeaderLabels.begin());
			if (!areHeaderLabelsSame) {
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
				std::cout << "WARNING: dataFile[" << std::to_string(i) << "] has <" << fileHeadersDebugString << "> while the previous headers were <" << allHeadersDebugString << ">." << std::endl;
			}
		}
		// Update the header labels
		headerLabels = fileHeaderLabels;

		// For each line the returned set of lines for a given file:
		for each (LabjackDataFileLine aLineObject in tempLines)
		{
			// Check the variables for each line. They should be the same within a file, but they may differ across files.
			numVariables = aLineObject.values.size();
			if (numVariables != maxNumVariables) {
				if (i > 0) {
					// If it's not the first file/line found
					std::cout << "dataFile[" << std::to_string(i) << "] has " << std::to_string(numVariables) << " variables while previous files only had " << std::to_string(maxNumVariables) << ". Need to adjust all with fewer variables to max." << std::endl;
				}
				maxNumVariables = max(maxNumVariables, numVariables);
			}

			this->milliseconds_since_epoch.push_back(aLineObject.milliseconds_since_epoch);
			this->values.push_back(aLineObject.values);
		} // end for each line
	} // end for each file
	//TODO: perhaps have a "reloading" event

	// Build arrays to hold the state change events for each variable
	for (int j = 0; j < maxNumVariables; j++) {
		std::vector<std::pair<unsigned long long, double>> newVariableVect;
		this->variableEventVectors.push_back(newVariableVect);
	}


	// Compute Differences and such
	int numSamples = this->milliseconds_since_epoch.size();
	if (numSamples < 2) { return; }



	//unsigned long long prev_time = this->milliseconds_since_epoch[0];
	//std::vector<double> prev_values = this->values[0];
	unsigned long long prev_time = this->milliseconds_since_epoch[0];
	std::vector<double> prev_values = this->values[0];

	unsigned long long curr_time;
	std::vector<double> curr_values;

	std::vector<double> temp_output_values;

	//std::vector<unsigned long long> output_date_times;
	//std::vector<std::vector<double>> output_values;

	bool shouldIncludeCurrentLine = false;
	// Loop through all the samples (lines)
	for (int i = 0; i < numSamples; i++)
	{
		shouldIncludeCurrentLine = false;
		temp_output_values.clear();

		curr_time = this->milliseconds_since_epoch[i];
		curr_values = this->values[i];

		//unsigned long long curr_time_diff = curr_time - prev_time;

		//TODO: deal with the first sample. 
		std::vector<double> currDiffVect = LabjackHelpers::computeDelta(curr_values, prev_values);

		// Loop through the variables
		for (int j = 0; j < maxNumVariables; j++) {
			//double currDiff = curr_values[j] - prev_values[j];
			double currDiff = currDiffVect[j];
			if (currDiff < -0.01) {
				// Include the value because it's a falling_edge (transition to negative values)
				shouldIncludeCurrentLine = true;
				temp_output_values.push_back(1.0);
				// Add the event timestamp and value to the appropriate vector
				this->variableEventVectors[j].push_back(make_pair(curr_time, curr_values[j]));
			}
			else {
				temp_output_values.push_back(0.0);
			}
		}

		// Output the line if wanted
		if (shouldIncludeCurrentLine) {
			this->output_milliseconds_since_epoch.push_back(curr_time);
			this->output_values.push_back(temp_output_values);
		}

		prev_time = curr_time;
		prev_values = curr_values;
	} // end sample for loop

	// Set the header labels
	this->headerLabels_ = headerLabels;
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

//findDataFiles(): called first, to find the .csv data files in the output directory.
void BehavioralBoxHistoricalData::findDataFiles()
{
	//TODO: It may be smarter to do this differntly when the dataFiles objects start containing actually loaded data.
	// Any given data file can potentially have its contents change without updating its filename. Maybe do some smart caching and looking at modification times, or look for existing monitoring solutions.
	this->dataFiles_.clear();
	this->dataFiles_ = BehavioralBoxHistoricalData::findDataFiles(this->dataFilesSearchDirectory_, this->labjackSerialNumber_);
	// Sort the data files by decending recency:
	this->sort();
}

//reloadDataFiles(): called after findDataFiles()
void BehavioralBoxHistoricalData::reloadDataFiles()
{
	for (int i = 0; i < this->dataFiles_.size(); i++)
	{
		this->dataFiles_[i].reloadContents();
	}
}
