#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "LabjackDataFile.h"

using namespace std;

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
	//ifstream is("bigfile.txt", ios::binary);

	ifstream csvFile;
	csvFile.open(this->fullPath.c_str());

	if (!csvFile.is_open())
	{
		cout << "Path Wrong!!!!" << endl;
		this->hadError = true;
		return false;
	}

	unsigned long long temp_milliseconds_since_epoch;
	std::vector<double> temp_values;
	char* endptr = nullptr;
	
	string line;
	vector <string> vec;

	// Read the header line
	getline(csvFile, line); // skip the 1st line
	if (line.empty()) // skip empty lines:
	{
		cout << "Empty file, no header!!!!" << endl;
		this->hadError = true;
		return false;
	}
	else {
		istringstream iss(line);
		string lineStream;
		string::size_type sz;
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
			//cout << "empty line!" << endl;
			continue;
		}

		istringstream iss(line);
		string lineStream;
		string::size_type sz;
		LabjackDataFileLine tempDataFileLine = LabjackDataFileLine();

		vector <long double> row;
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
	this->sort();

	//TODO: perhaps record the last read time or something

	return true;
}
