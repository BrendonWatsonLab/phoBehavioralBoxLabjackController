#include <limits>
#include <iostream>
#include <string>
#include "BehavioralBoxHistoricalData.h"
#include "FilesystemHelpers.h"
#include <sstream>

BehavioralBoxHistoricalData::BehavioralBoxHistoricalData(std::string searchDirectory, int labjackSerialNumber, std::string boxID)
{
	this->labjackSerialNumber_ = labjackSerialNumber;
	this->dataFilesSearchDirectory_ = searchDirectory;
	this->boxID_ = boxID;

	// TODO: find data files in the background
	this->findDataFiles();
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
		// Check if the serial number matches the search serial number
		if (fileNameParts[2] != expectedSerialNumberString) {
			// Serial number doesn't match
			continue;
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
		LabjackDataFile currFile = LabjackDataFile(foundFiles[i], labjackSerialNumber, millisecondsComponent);
		outputVector.push_back(currFile);
	}
	return outputVector;
}

void BehavioralBoxHistoricalData::findDataFiles()
{
	//TODO: It may be smarter to do this differntly when the dataFiles objects start containing actually loaded data.
	// Any given data file can potentially have its contents change without updating its filename. Maybe do some smart caching and looking at modification times, or look for existing monitoring solutions.
	this->dataFiles_.clear();
	this->dataFiles_ = BehavioralBoxHistoricalData::findDataFiles(this->dataFilesSearchDirectory_, this->labjackSerialNumber_);
}
