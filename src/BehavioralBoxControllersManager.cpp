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

bool BehavioralBoxControllersManager::waitForFoundLabjacks()
{
	bool stillWaitingToFindLabjacks = true;
	int character;
	do {
		// Find the labjacks
		this->foundLabjacks_ = LabjackHelpers::findAllLabjacks();

		if (this->foundLabjacks_.size() == 0) {
			printf("No labjacks found!!\n");
			printf("Make sure Kipling and all other software using the Labjack is closed, and that the labjack is plugged in via USB.\n");
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
			// Otherwise labjacks have found, move forward with the program.
			// Iterate through all found Labjacks
			for (int i = 0; i < this->foundLabjacks_.size(); i++) {
				this->foundLabjacks_[i]->syncDeviceTimes();
				this->foundLabjacks_[i]->updateVisibleLightRelayIfNeeded();
			}
			stillWaitingToFindLabjacks = false;
		}

	} while (stillWaitingToFindLabjacks == true);
	// Returns true to indicate that labjacks have been found and we should move forward with the program.
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

void BehavioralBoxControllersManager::loadHistoricalData()
{
	this->historicalData_.clear();
	for (int i = 0; i < this->getActiveLabjacks().size(); i++) {
		
		//std::vector<LabjackDataFile> currLabjackDataFile = this->findDataFiles(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
		BehavioralBoxHistoricalData currHistoryManager = BehavioralBoxHistoricalData(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
		this->historicalData_.push_back(currHistoryManager);
	}
}
