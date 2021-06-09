#include <limits>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "FilesystemHelpers.h"
//Manhong-001 get the experimentName in the ini file
#include "ConfigurationManager.h"

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

template<bool RECURSIVE>
inline std::vector<fs::path> FilesystemHelpers::dir_list(fs::path dir, std::regex folder_name_pattern)
{
	std::vector<fs::path> result;
	using iterator = std::conditional< RECURSIVE,
		fs::recursive_directory_iterator, fs::directory_iterator >::type;

	const iterator end;
	for (iterator iter{ dir }; iter != end; ++iter)
	{
		if (!fs::is_directory(*iter)) {
			continue;
		}
		const std::string folder_name = iter->path().filename().string();
		if (std::regex_match(folder_name, folder_name_pattern)) {
			result.push_back(*iter);
		}
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
	const bool should_warn_on_non_matching_file = false;

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
		// std::stringstream class check1 
		std::stringstream currNameSplitterStream(currFileBasename);
		// Clear the name parts for the next file
		fileNameParts.clear();
		// Tokenizing w.r.t. underscore '_' 
		while (getline(currNameSplitterStream, intermediate, '_'))
		{
			fileNameParts.push_back(intermediate);
		}
		if (fileNameParts.size() != 4) {
			if (should_warn_on_non_matching_file) {
				std::cout << "Warning: File " << currFileBasename << " does not match the expected format (out_file_s{ SERIAL_NUMBER }_{ MILLISECONDS_SINCE_EPOCH }). Skipping." << std::endl;
			}
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
				//TODO: DO I need "New" here?
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


// get the labjack-serial-split map of files and then just combine them into a vector
std::vector<LabjackDataFile> FilesystemHelpers::findAllDataFiles(std::string searchDirectory)
{
	std::vector<LabjackDataFile> resultFiles;
	std::map<int, std::vector<LabjackDataFile>> labjackSerialSplitFiles = findDataFiles(searchDirectory);
	for (const auto& activeFilePathPair : labjackSerialSplitFiles) {
		int curr_labjackSerialNumber = activeFilePathPair.first;
		std::vector<LabjackDataFile> curr_found_files = activeFilePathPair.second;
		resultFiles.insert(std::end(resultFiles), std::begin(curr_found_files), std::end(curr_found_files));
	}
	return resultFiles;
}

// Creates a directory only if needed
bool FilesystemHelpers::createDirectory(std::string path)
{
	if (path.empty()) {
		return false;
	}
	if (fs::exists(path)) {
		//Directory already exists
		return false;
	}
	else {
		return fs::create_directories(path);
	}

	
}

bool FilesystemHelpers::fileExists(std::string path)
{
	if (path.empty()) {
		return false;
	}
	return fs::exists(path);
}

FilesystemHelpers::FilesystemItemStatus FilesystemHelpers::filesystemItemStatus(std::string path)
{
	if (path.empty()) {
		return FilesystemHelpers::FilesystemItemStatus::NonExistant;
	}
	if (!fs::exists(path)) {
		return FilesystemHelpers::FilesystemItemStatus::NonExistant;
	}
	if (fs::is_directory(path))
	{
		return FilesystemHelpers::FilesystemItemStatus::Directory;
	}
	if (fs::is_regular_file(path))
	{
		return FilesystemHelpers::FilesystemItemStatus::File;
	}
	return FilesystemItemStatus::OtherExtant;
}


std::map<int, fs::path> FilesystemHelpers::findBehavioralBoxDataFolders(fs::path dir)
{
	std::map<int, fs::path> full_path_result_map;

	std::vector<fs::path> box_folders = dir_list<false>(dir, folder_bb_folder_regex);
	for each (fs::path a_box_folder in box_folders)
	{
		const std::string curr_box_folder_name = a_box_folder.filename().string();
		std::smatch stringMatch;    // same as std::match_results<string::const_iterator> sm;
		std::regex_match(curr_box_folder_name, stringMatch, folder_bb_folder_regex);
		if (stringMatch.size() <= 1) {
			std::cout << "Couldn't parse number from " << curr_box_folder_name << ". It's not of the expected format \"BBXX\"." << std::endl;
			continue;
		}
		else {
			// numbersMatchString should be a string like "06". Try to find the animal folder with this same number
			std::string numbersMatchString = stringMatch[1];
			int currBBID = std::stoi(numbersMatchString);
			full_path_result_map[currBBID] = a_box_folder;
		}
	} // end for box folders

	return full_path_result_map;
}

//TODO: This currently returns the first matching animal folder found per box folder, regardless of experiment or cohort. 
std::map<int, fs::path> FilesystemHelpers::findActiveExperimentAnimalFolders(fs::path dir)
{
	//Manhong-002 get the experimentName in the ini file
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
	//std::vector<fs::path> full_path_results;
	std::map<int, fs::path> full_path_result_map;

	std::vector<fs::path> box_folders = dir_list<false>(dir, folder_bb_folder_regex);
	for each (fs::path a_box_folder in box_folders)
	{
		const std::string curr_box_folder_name = a_box_folder.filename().string();
		std::smatch stringMatch;    // same as std::match_results<string::const_iterator> sm;
		std::regex_match(curr_box_folder_name, stringMatch, folder_bb_folder_regex);
		if (stringMatch.size() <= 1) {
			std::cout << "Couldn't parse number from " << curr_box_folder_name << ". It's not of the expected format \"BBXX\"." << std::endl;
			continue;
		}
		else {
			// numbersMatchString should be a string like "06". Try to find the animal folder with this same number
			std::string numbersMatchString = stringMatch[1];
			int currBBID = std::stoi(numbersMatchString);
			//Manhong-003
			std::string matchExperimentStr = "BB" + numbersMatchString + "\\" + configMan->getLoadedConfig().experimentName + "\\";
			// Search recurrsively for the animal folders (TODO: this ignores experiment/cohort for now)
			std::vector<fs::path> found_animal_folders = dir_list<true>(a_box_folder, folder_animal_folder_regex);
			if (found_animal_folders.size() < 1)
			{
				// No animal folder found for this dir!
				std::cout << "WARNING: no animal folders in " << curr_box_folder_name << ". Skipping." << std::endl;
				continue;
			}
			else {
				// More than one animal folder found, try to find the one that matches the BB number!
				// TODO: in the future I should check if multiple found animal folders match the BB number (corresponding to the same animal in multiple experiments/cohorts).
				bool found_matching_animal_folder = false;
				for each (fs::path an_animal_folder in found_animal_folders)
				{
					//Manhong-004 use BB\d\d + the experimentName in ini file to filter found paths.
					if (an_animal_folder.parent_path().string().find(matchExperimentStr) == std::string::npos) continue;
					const std::string curr_animal_folder_name = an_animal_folder.filename().string();
					std::regex_match(curr_animal_folder_name, stringMatch, folder_animal_folder_regex);
					if (stringMatch.size() <= 1) {
						std::cout << "Couldn't parse number from " << curr_animal_folder_name << ". It's not of the expected format \"animal_XX\"." << std::endl;
						continue;
					}
					else {
						std::string animalNumbersMatchString = stringMatch[1];
						if (animalNumbersMatchString == numbersMatchString) {
							// Found the final desired path:
							//full_path_results.push_back(an_animal_folder);
							full_path_result_map[currBBID] = an_animal_folder;

							found_matching_animal_folder = true;
							std::cout << "FOUND: " << an_animal_folder << "." << std::endl;
							break; // exit the loop, we found the one. TODO: this needs to be changed to support multiple experiments/cohorts
						}
					}

				}
				if (!found_matching_animal_folder) {
					std::cout << "WARNING: no matching animal folder in " << curr_box_folder_name << ". Skipping." << std::endl;
					continue;
				}
				else {
					// found the single animal folder we're looking for: TODO: support multiple experiment/cohort in future.
					continue;
				}
				
			}

		}
	} // end for box folders

	//return full_path_results;
	return full_path_result_map;
}

