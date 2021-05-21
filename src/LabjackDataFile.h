#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>

//using namespace std;
namespace fs = std::filesystem;

struct LabjackDataFileLine {
	unsigned long long milliseconds_since_epoch;
	std::vector<double> values;

	// Can be called with std::sort(vec.begin(), vec.end());
	bool operator < (const LabjackDataFileLine& line) const
	{
		return (milliseconds_since_epoch < line.milliseconds_since_epoch);
	}
	// Can be called with std::sort(vec.begin(), vec.end(),greater<MyStruct>());
	bool operator > (const LabjackDataFileLine& line) const
	{
		return (milliseconds_since_epoch > line.milliseconds_since_epoch);
	}
};


struct LabjackDataFile {
	fs::path fullPath;
	std::string fileName;
	std::string baseName;
	//std::string extension;

	int owningLabjackSerialNumber = 0;
	unsigned long long millisecondsSinceEpoch = 0;

	// This could parse its variables exclusively from fullPath, but that would be less efficient for the "BehavioralBoxControllersManager::findDataFiles((std::string searchDirectory, int labjackSerialNumber, ...)" approach which can skip a file if it doesn't match the labjack
	// Instead this is just used mostly as a convenience storage class to be returned from that function.
	LabjackDataFile(fs::path fullPath, int owningLabjackSerialNumber, unsigned long long millisecondsSinceEpoch) {
		this->fullPath = fullPath;
		this->fileName = this->fullPath.filename().string();
		this->baseName = this->fullPath.stem().string(); // Stem returns the filename without the extension.
		//this->extension = this->fullPath.extension();
		this->owningLabjackSerialNumber = owningLabjackSerialNumber;
		this->millisecondsSinceEpoch = millisecondsSinceEpoch;
	}

	// Can be called with std::sort(vec.begin(), vec.end());
	bool operator < (const LabjackDataFile& file) const
	{
		return (millisecondsSinceEpoch < file.millisecondsSinceEpoch);
	}
	// Can be called with std::sort(vec.begin(), vec.end(),greater<MyStruct>());
	bool operator > (const LabjackDataFile& file) const
	{
		return (millisecondsSinceEpoch > file.millisecondsSinceEpoch);
	}

	// Reloads the contents from the file
	bool reloadContents();

	std::vector<std::string> getParsedHeaderLabels() { return this->headerLabels_; };
	std::vector<LabjackDataFileLine> getParsedLines() { return this->lineValues_; };

	std::chrono::time_point<std::chrono::system_clock> getFileTimestamp();



private:
	std::vector<std::string> headerLabels_;
	std::vector<LabjackDataFileLine> lineValues_;
	bool hadError = false;

	void sort() {
		std::sort(lineValues_.begin(), lineValues_.end());
	}

};