#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "BehavioralBoxControllersManager.h"
#include "LabjackHelpers.h"

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
		cout << "Found " << newlyFoundAdditionalLabjacks.size() << " new labjacks!" << endl;
		// Iterate through all newly found labjacks and append them to the list of found labjacks
		for (int i = 0; i < newlyFoundAdditionalLabjacks.size(); i++) {
			this->addLabjack(newlyFoundAdditionalLabjacks[i]);
		}
	}
	else {
		cout << "Found no new labjacks." << endl;
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
				cout << "\t Continuing without a Labjack..." << endl;
				this->stillWaitingToFindLabjacks_ = false;
			}
			else {
				if (is_interactive_mode)
				{
					// Interactive Mode
					cout << "\t Press [Q] to quit or any other key to rescan for Labjacks." << endl;
					// Read a character from the keyboard
					character = _getch();
					character = toupper(character);
					if (character == 'Q') {
						// Returns false to indicate that the user gave up.
						cout << "\t Quitting..." << endl;
						return false;
					}
					else {
						//std::this_thread::sleep_for(std::chrono::seconds(1));
						cout << "\t Refreshing Labjacks..." << endl;
						continue;
					}
				}
				else {
					cout << "\t Searching again for Labjacks in 5 seconds..." << endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					cout << "\t Refreshing Labjacks..." << endl;
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

				
			}
		}
	}
}

BehavioralBoxHistoricalData BehavioralBoxControllersManager::getHistoricalData(int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch)
{
	std::vector<LabjackDataFile> currLabjackAssociatedFilesVector;
	try {
		// Try to find a previously existing vector of LabjackDataFiles in the map by indexing with the parsed serial number.
		// "at(...)" is used instead of the traditional index notation ("[...]") because it throws an exception if it doesn't exist instead of adding it silently so we can create the vector if needed.
		currLabjackAssociatedFilesVector = this->labjackDataFilesMap_.at(labjackSerialNumber);
	}
	catch (...) {
		// Map entry doesn't already exist. Create an empty vector (could also rescan for files and regenerate the map).
		currLabjackAssociatedFilesVector = std::vector<LabjackDataFile>();
	}

	// Filter the files to be within the range if needed
	if ((startMillisecondsSinceEpoch > 0) || (endMillisecondsSinceEpoch < std::numeric_limits<unsigned long long>::max())) {
		// Loop through the data files and filter based on whether their timestamp is within the range
		std::vector<LabjackDataFile> outputVector;
		for each (auto aDataFile in currLabjackAssociatedFilesVector)
		{
			// Check whether it's within the time range the user provided
			if ((aDataFile.millisecondsSinceEpoch < startMillisecondsSinceEpoch) || (aDataFile.millisecondsSinceEpoch > endMillisecondsSinceEpoch)) {
				// It's outisde the available range
				continue;
			}
			else {
				// Otherwise it's in the range and should be added
				outputVector.push_back(aDataFile);
			}
		}
		// return the filtered range
		return BehavioralBoxHistoricalData(this->dataFilesSearchDirectory_, labjackSerialNumber, outputVector);
	}
	else {
		// return the unfiltered range (all time).
		return BehavioralBoxHistoricalData(this->dataFilesSearchDirectory_, labjackSerialNumber, currLabjackAssociatedFilesVector);
	}
}

BehavioralBoxHistoricalData BehavioralBoxControllersManager::getHistoricalData(int labjackSerialNumber)
{
	return this->getHistoricalData(labjackSerialNumber, 0, std::numeric_limits<unsigned long long>::max());
}
