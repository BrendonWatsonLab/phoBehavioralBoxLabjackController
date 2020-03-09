#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "LabjackDataFile.h"
#include "FormattingHelper.h"

//using namespace std;

bool LabjackDataFile::reloadContents()
{
	// Reset:
	this->lineValues_.clear();
	this->headerLabels_.clear();

	//std::string line;
	//while (std::getline(stream, line)) {
	//	std::istringstream s(line);
	//	std::string field;
	//	while (getline(s, field, ','))
	//}
	//ifstream is("bigfile.txt", std::ios::binary);

	std::ifstream csvFile;
	csvFile.open(this->fullPath.c_str());

	if (!csvFile.is_open())
	{
		std::cout << "Path Wrong!!!!" << std::endl;
		this->hadError = true;
		return false;
	}

	unsigned long long temp_milliseconds_since_epoch;
	std::vector<double> temp_values;
	char* endptr = nullptr;
	
	std::string line;
	std::vector <std::string> vec;

	// Read the header line
	getline(csvFile, line); // skip the 1st line
	if (line.empty()) // skip empty lines:
	{
		std::cout << "Empty file, no header!!!!" << std::endl;
		this->hadError = true;
		return false;
	}
	else {
		std::istringstream iss(line);
		std::string lineStream;
		std::string::size_type sz;
		// Add the item to the header labels stream
		while (getline(iss, lineStream, ','))
		{
			this->headerLabels_.push_back(lineStream);
		}
	}

	// Read the remaining lines:
	while (getline(csvFile, line))
	{
		if (line.empty()) // skip empty lines:
		{
			//std::cout << "empty line!" << std::endl;
			continue;
		}

		std::istringstream iss(line);
		std::string lineStream;
		std::string::size_type sz;
		LabjackDataFileLine tempDataFileLine = LabjackDataFileLine();

		std::vector <long double> row;
		int columnIndex = 0;
		while (getline(iss, lineStream, ','))
		{
			if (columnIndex == 0) {
				// The first column is the timestamp
				temp_milliseconds_since_epoch = strtoull(lineStream.c_str(), &endptr, 10);
				tempDataFileLine.milliseconds_since_epoch = temp_milliseconds_since_epoch;
			}
			else {
				long double currValue = stold(lineStream, &sz);  // convert to double
				tempDataFileLine.values.push_back(currValue);
				//row.push_back(stold(lineStream, &sz));
			}
			columnIndex++;
		} // end get token in line while
		// Add the file line object to the list of them
		this->lineValues_.push_back(tempDataFileLine);
	} // end get lines in file while
	
	// Close the file when done
	csvFile.close();

	// Sort the data lines (should already be sorted)...
	//this->sort();

	//TODO: perhaps record the last read time or something

	return true;
}

std::chrono::time_point<std::chrono::system_clock> LabjackDataFile::getFileTimestamp()
{
	return FormattingHelper::date_from_milliseconds_since_epoch(this->millisecondsSinceEpoch);
}
