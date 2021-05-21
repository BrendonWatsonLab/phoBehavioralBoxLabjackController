#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>    // std::min_element, std::max_element
#include <conio.h>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "BehavioralBoxControllersManager.h"
#include "LabjackHelpers.h"
#include "FormattingHelper.h"

BehavioralBoxControllersManager::BehavioralBoxControllersManager()
{
	this->thread_ = std::thread(std::bind(&BehavioralBoxControllersManager::run, this));
}

BehavioralBoxControllersManager::~BehavioralBoxControllersManager()
{
	this->shouldStop_ = true;
	// As the thread is using members from this object
	  // We can not let this obect be destroyed until
	  // the thread finishes executing.
	this->thread_reloadHistoricalData_.join();
	this->thread_.join();

}

void BehavioralBoxControllersManager::connect(Client* client, const std::function<void()>& function)
{
	std::unique_lock<std::mutex> lock(mutex_);
#if LAUNCH_WEB_SERVER
	connections_.push_back(Connection(Wt::WApplication::instance()->sessionId(), client, function));
#endif // LAUNCH_WEB_SERVER
}

void BehavioralBoxControllersManager::disconnect(Client* client)
{
	std::unique_lock<std::mutex> lock(mutex_);

	for (unsigned i = 0; i < connections_.size(); ++i) {
		if (connections_[i].client == client) {
			connections_.erase(connections_.begin() + i);
			return;
		}
	}
	assert(false);
}

bool BehavioralBoxControllersManager::scanForNewLabjacks()
{
	int previouslyFoundLabjackSerialNumbers[max_number_labjacks] = {};
	int numberPreviouslyFoundLabjacks = this->numberActiveLabjacks_;
	for (int i = 0; i < numberPreviouslyFoundLabjacks; i++) {
		previouslyFoundLabjackSerialNumbers[i] = this->foundLabjacks_[i]->getSerialNumber();
	}

	// Find the labjacks
	std::vector<BehavioralBoxLabjack*> newlyFoundAdditionalLabjacks = LabjackHelpers::findAllLabjacks(previouslyFoundLabjackSerialNumbers, numberPreviouslyFoundLabjacks);

	if (newlyFoundAdditionalLabjacks.size() > 0) {
		std::cout << "Found " << newlyFoundAdditionalLabjacks.size() << " new labjacks!" << std::endl;
		// Iterate through all newly found labjacks and append them to the list of found labjacks
		for (int i = 0; i < newlyFoundAdditionalLabjacks.size(); i++) {
			this->addLabjack(newlyFoundAdditionalLabjacks[i]);
		}
	}
	else {
		std::cout << "Found no new labjacks." << std::endl;
	}

	return false;
}

bool BehavioralBoxControllersManager::waitForFoundLabjacks()
{

	this->stillWaitingToFindLabjacks_ = true;
	bool is_interactive_mode = false; // Whether the program waits for the user's input, or whether it continues retrying automatically.
	int character;
	do {
		// Find the labjacks
		std::vector<BehavioralBoxLabjack*> newlyFoundAdditionalLabjacks = LabjackHelpers::findAllLabjacks();
		//this->foundLabjacks_ = LabjackHelpers::findAllLabjacks();

		if (newlyFoundAdditionalLabjacks.size() == 0) {
			printf("No labjacks found!!\n");
			printf("Make sure Kipling and all other software using the Labjack is closed, and that the labjack is plugged in via USB.\n");

			if (this->configMan->getLoadedConfig().continue_without_labjacks) {
				std::cout << "\t Continuing without a Labjack..." << std::endl;
				this->stillWaitingToFindLabjacks_ = false;
			}
			else {
				if (is_interactive_mode)
				{
					// Interactive Mode
					std::cout << "\t Press [Q] to quit or any other key to rescan for Labjacks." << std::endl;
					// Read a character from the keyboard
					character = _getch();
					character = toupper(character);
					if (character == 'Q') {
						// Returns false to indicate that the user gave up.
						std::cout << "\t Quitting..." << std::endl;
						return false;
					}
					else {
						//std::this_thread::sleep_for(std::chrono::seconds(1));
						std::cout << "\t Refreshing Labjacks..." << std::endl;
						continue;
					}
				}
				else {
					std::cout << "\t Searching again for Labjacks in 5 seconds..." << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					std::cout << "\t Refreshing Labjacks..." << std::endl;
					continue;
				}
				
			} // CONTINUE_WITHOUT_LABJACKS
		}
		else {
			// Otherwise labjacks have found, move forward with the program.
			// Iterate through all found Labjacks and add them
			for (int i = 0; i < newlyFoundAdditionalLabjacks.size(); i++) {
				this->addLabjack(newlyFoundAdditionalLabjacks[i]);
			}
			this->stillWaitingToFindLabjacks_ = false;
		}

	} while (this->stillWaitingToFindLabjacks_ == true);
	// Returns true to indicate that labjacks have been found and we should move forward with the program.
	return true;
}

// Returns true only if it's ready for a connection from the client
bool BehavioralBoxControllersManager::isReady()
{
	if (this->stillWaitingToFindLabjacks_ || this->shouldStop_) {
		return false;
	}
	else {
		return true;
	}
}


bool BehavioralBoxControllersManager::addLabjack(BehavioralBoxLabjack* newLabjack)
{
	newLabjack->syncDeviceTimes();
	newLabjack->updateVisibleLightRelayIfNeeded();
	this->foundLabjacks_.push_back(newLabjack);
	this->numberActiveLabjacks_ = this->foundLabjacks_.size();
	return true;
}

void BehavioralBoxControllersManager::run()
{
	/*
   * This method simulates changes to the data that happen in a background
   * thread.
   */
	for (;;) {
		//std::this_thread::sleep_for(std::chrono::seconds(1));

		if (this->shouldStop_) {
			return;
		}

		{
			std::unique_lock<std::mutex> lock(mutex_);
			//++counter_;
			//TODO: Update the number of labjacks and such
			// **HERE**


			// **HERE**
			/* This is where we notify all connected clients. */
			for (unsigned i = 0; i < connections_.size(); ++i) {
				Connection& c = connections_[i];
#if LAUNCH_WEB_SERVER
				Wt::WServer::instance()->post(c.sessionId, c.function);
#endif // LAUNCH_WEB_SERVER

				
			}
		}
	}
}


//TODO: currently doesn't make use of the individual Labjack object's output directory, and instead uses this class' member variable dataFilesSearchDirectory_
void BehavioralBoxControllersManager::reloadHistoricalData()
{
	if (!this->configMan->getLoadedConfig().enableHistoricalDataLoading) {
		// Historical Data loading is disabled, just return.
		//TODO: validate that this is acceptable.
		return;
	}
	//TODO: do on background thread
	// Find the folders to search:

	fs::path curr_search_dir = this->dataFilesSearchDirectory_;
	// Get the children search dirs:
	std::map<int, fs::path> foundBoxPaths = FilesystemHelpers::findBehavioralBoxDataFolders(curr_search_dir);
	//std::vector<int> found_box_ids;

	// Get the maximum found box ID from the found paths:
	int maximum_found_box_id = -1;
	for (std::map<int, fs::path>::iterator it = foundBoxPaths.begin(); it != foundBoxPaths.end(); ++it) {
		maximum_found_box_id = std::max(maximum_found_box_id, it->first);
		//found_box_ids.push_back(it->first);
		//std::cout << it->first << "\n";
	}
	std::cout << "Maximum found box id is " << maximum_found_box_id << "." << std::endl;

	//maximum_found_box_id
	this->behavioralBoxEventDataFilesMap_ = std::map<int, std::vector<LabjackDataFile>>();
	// Iterate through the BBIDs to initialize the vectors
	for (size_t i = 0; i < maximum_found_box_id; i++)
	{
		int curr_box_id = i + 1;
		this->behavioralBoxEventDataFilesMap_[curr_box_id] = std::vector<LabjackDataFile>();
	}

	///Finds the correct animal folder within each found BB folder
	std::map<int, fs::path> foundSearchPaths = FilesystemHelpers::findActiveExperimentAnimalFolders(curr_search_dir);
	for (const auto& activeBoxSearchPathPair : foundSearchPaths) {
		int curr_bbID = activeBoxSearchPathPair.first;
		fs::path curr_search_path = activeBoxSearchPathPair.second;
		// Search that path for files:
		this->behavioralBoxEventDataFilesMap_[curr_bbID] = FilesystemHelpers::findAllDataFiles(curr_search_path.string());

	}

	//TODO: change to sort by search paths, not labjack serials
	//labjackDataFilesMap_ = FilesystemHelpers::findDataFiles(this->dataFilesSearchDirectory_);

	if (this->shouldStop_) {
		// Not ready.
		return;
	}
	// Clear the old historical data
	this->historicalData_.clear();
	// Iterate through the search paths one more time and build historical data objects (BehavioralBoxHistoricalData) from the found data files
	for (const auto& activeBoxSearchPathPair : foundSearchPaths) {
		int curr_bbID = activeBoxSearchPathPair.first;
		std::string curr_bbID_String = FormattingHelper::format_two_digit_string(curr_bbID);
		fs::path curr_search_path = activeBoxSearchPathPair.second;
		BehavioralBoxHistoricalData currHistoryData = BehavioralBoxHistoricalData(curr_search_path.string(), -1, curr_bbID_String, this->behavioralBoxEventDataFilesMap_[curr_bbID]);
		this->historicalData_.push_back(currHistoryData);
	}

	//if (this->getActiveLabjacks().size() > 0) {
	//	// Loop through the active labjacks and get the historical data corresponding to them.
	//	for (int i = 0; i < this->getActiveLabjacks().size(); i++) {
	//		BehavioralBoxHistoricalData currHistoryData = this->getHistoricalData(this->getActiveLabjacks()[i]->getSerialNumber());
	//		this->historicalData_.push_back(currHistoryData);
	//	}
	//}
	//else {
	//	// No actual labjacks, load **everything**:
	//	//this->historicalData_ = BehavioralBoxControllersManager::loadAllHistoricalData();
	//	for (const auto& bbFilesPair : this->behavioralBoxEventDataFilesMap_) {
	//		int curr_bbID = bbFilesPair.first;
	//		std::string curr_bbID_String = FormattingHelper::format_two_digit_string(curr_bbID);


	//		BehavioralBoxHistoricalData currHistoryData = BehavioralBoxHistoricalData(this->dataFilesSearchDirectory_, labjackFilesPair.first, labjackFilesPair.second);
	//		this->historicalData_.push_back(currHistoryData);
	//	}
	//}

}

std::vector<std::string> BehavioralBoxControllersManager::exportHistoricalDataAsCSV(std::string path, std::string base_filename)
{
	std::string fullPrefixPath = path + base_filename + "_";
	std::vector<std::string> outputPaths;
	for (int i = 0; i < this->historicalData_.size(); i++)
	{
		//TODO: this exports all historical data to the same file... is this okay?
		std::string currFullOutputPath = fullPrefixPath + std::to_string(i) + ".csv";
		outputPaths.push_back(currFullOutputPath);
		this->historicalData_[i].exportAsCSV(currFullOutputPath);
	}
	return outputPaths;
}


void BehavioralBoxControllersManager::serverGetAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback)
{
	auto updatedData = HistoricalDataLoadingEvent(this->historicalData_);
	completionCallback(updatedData);
}

// called by the BoxControllerWebDataServer to request updated historical data
void BehavioralBoxControllersManager::serverLoadAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback)
{
	// std::lock_guard
	this->thread_reloadHistoricalData_ = std::move(std::thread([=]() {
		this->reloadHistoricalData();
		auto updatedData = HistoricalDataLoadingEvent(this->historicalData_);
		completionCallback(updatedData);
		return 1;
	}));
}
