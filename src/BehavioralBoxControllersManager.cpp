#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>    // std::min_element, std::max_element
#include <conio.h>
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
	this->thread_.join();

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

		
		}
	}
}
