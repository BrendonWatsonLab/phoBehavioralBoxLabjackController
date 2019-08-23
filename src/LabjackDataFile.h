#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>

//using namespace std;
namespace fs = std::experimental::filesystem;

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

};