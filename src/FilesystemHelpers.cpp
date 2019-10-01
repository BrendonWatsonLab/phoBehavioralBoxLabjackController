#include <limits>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "FilesystemHelpers.h"

template<bool RECURSIVE>
inline std::vector<fs::path> FilesystemHelpers::file_list(fs::path dir, std::regex ext_pattern)
{
	std::vector<fs::path> result;

	using iterator = std::conditional< RECURSIVE,
		fs::recursive_directory_iterator, fs::directory_iterator >::type;

	const iterator end;
	for (iterator iter{ dir }; iter != end; ++iter)
	{
		const std::string extension = iter->path().extension().string();
		if (fs::is_regular_file(*iter) && std::regex_match(extension, ext_pattern)) result.push_back(*iter);
	}

	return result;
}

std::vector<fs::path> FilesystemHelpers::scan_csv_files(fs::path dir)
{
	return FilesystemHelpers::file_list<false>(dir, csv_files);
}

std::vector<fs::path> FilesystemHelpers::rscan_csv_files(fs::path dir)
{
	return file_list<true>(dir, csv_files);
}

std::map<int, std::vector<LabjackDataFile>> FilesystemHelpers::findDataFiles(std::string searchDirectory)
{
	// Looks for files of the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}.csv"
	// activeLabjackSerialNumber: used to hold the parsed LabjackSerial Number
	int activeLabjackSerialNumber = 0;
	std::map<int, std::vector<LabjackDataFile>> labjackDataFilesMap;
												
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

		// Try to parse the string to the labjack serial number
		const std::string serialOnlyString(fileNameParts[2].begin() + 1, fileNameParts[2].end()); // Get the string skipping the "s" at the beginning
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

		std::vector<LabjackDataFile> currLabjackAssociatedFilesVector;
		// Try to find a previously existing vector of LabjackDataFiles in the map by indexing with the parsed serial number.
		if (labjackDataFilesMap.count(activeLabjackSerialNumber) > 0) {
			// "at(...)" is used instead of the traditional index notation ("[...]") because it throws an exception if it doesn't exist instead of adding it silently so we can create the vector if needed.
			try {
				currLabjackAssociatedFilesVector = labjackDataFilesMap.at(activeLabjackSerialNumber);
			}
			catch (...) {
				// Map entry doesn't already exist. Create a new vector to add to the map
				currLabjackAssociatedFilesVector = std::vector<LabjackDataFile>();
			}
		}
		else {
			// Map entry doesn't already exist. Create a new vector to add to the map
			currLabjackAssociatedFilesVector = std::vector<LabjackDataFile>();
		}

		// It meets all criteria to be returned. Add it to the output vector
		LabjackDataFile currFile = LabjackDataFile(foundFiles[i], activeLabjackSerialNumber, millisecondsComponent);
		currLabjackAssociatedFilesVector.push_back(currFile);

		// Update the map with the new or updated vector
		labjackDataFilesMap[activeLabjackSerialNumber] = currLabjackAssociatedFilesVector; // TODO: This might inefficiently do a copy every time, I'm unsure.
		
	}
	return labjackDataFilesMap;
}

//std::string FilesystemHelpers::getExePath()
//{
//#ifdef _WIN32
//	char result[MAX_PATH];
//	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
//#else
//	char result[PATH_MAX];
//	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//	return std::string(result, (count > 0) ? count : 0);
//#endif
//}
