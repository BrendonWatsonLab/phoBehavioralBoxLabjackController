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
	this->thread_.join();
}

void BehavioralBoxControllersManager::connect(Client* client, const std::function<void()>& function)
{
	std::unique_lock<std::mutex> lock(mutex_);
	connections_.push_back(Connection(Wt::WApplication::instance()->sessionId(), client, function));
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
	int character;
	do {
		// Find the labjacks
		std::vector<BehavioralBoxLabjack*> newlyFoundAdditionalLabjacks = LabjackHelpers::findAllLabjacks();
		//this->foundLabjacks_ = LabjackHelpers::findAllLabjacks();

		if (newlyFoundAdditionalLabjacks.size() == 0) {
			printf("No labjacks found!!\n");
			printf("Make sure Kipling and all other software using the Labjack is closed, and that the labjack is plugged in via USB.\n");

#if CONTINUE_WITHOUT_LABJACKS
			this->stillWaitingToFindLabjacks_ = false;
#else
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
#endif // CONTINUE_WITHOUT_LABJACKS
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
		std::this_thread::sleep_for(std::chrono::seconds(1));

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
				Wt::WServer::instance()->post(c.sessionId, c.function);
			}
		}
	}
}


void BehavioralBoxControllersManager::reloadHistoricalData()
{
	if (!this->isReady()) {
		// Not ready.
		return;
	}
	// Clear the old historical data
	this->historicalData_.clear();

	if (this->getActiveLabjacks().size() > 0) {
		// Loop through the active labjacks and get the historical data corresponding to them.
		for (int i = 0; i < this->getActiveLabjacks().size(); i++) {
			//std::vector<LabjackDataFile> currLabjackDataFile = this->findDataFiles(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
			BehavioralBoxHistoricalData currHistoryManager = BehavioralBoxHistoricalData(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
			this->historicalData_.push_back(currHistoryManager);
		}
	}
	else {
		// No actual labjacks, load everything:
		this->historicalData_ = BehavioralBoxControllersManager::loadHistoricalData();
	}
}

std::vector<BehavioralBoxHistoricalData> BehavioralBoxControllersManager::loadHistoricalData()
{
	std::string outputDirectory = "output_data/";
	std::vector<BehavioralBoxHistoricalData> outputVector;
	BehavioralBoxHistoricalData currHistoryManager = BehavioralBoxHistoricalData(outputDirectory, 0);
	outputVector.push_back(currHistoryManager);
	//TODO: should sort the output
	return outputVector;
}
