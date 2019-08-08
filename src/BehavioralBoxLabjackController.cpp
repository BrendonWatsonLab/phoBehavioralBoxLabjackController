/**
 * Name: BehavioralBoxLabjackController.cpp
 * Desc: Main file
**/

//#ifdef _WIN32
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#else
//#include <arpa/inet.h>  // For inet_ntoa()
//#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <LabJackM.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <conio.h>

//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"
#include "BehavioralBoxLabjack.h"

#include "LabjackHelpers.h"

// Vector of Labjack Objects
std::vector<BehavioralBoxLabjack*> foundLabjacks;


//// Scheduler
#include "External/Scheduler/Scheduler.h"
// Make a new scheduling object.
  // Note: s cannot be moved or copied
Bosma::Scheduler s(max_n_threads);

// FUNCTION PROTOTYPES:

void runTopOfHourUpdate();
void runTopOfMinuteUpdate();
void runTopOfSecondUpdate();
void runPollingLoopUpdate();

void printCommandsMenu();


int main()
{
	// Find the labjacks
	foundLabjacks = LabjackHelpers::findAllLabjacks();

	if (foundLabjacks.size() == 0) {
		printf("No labjacks found!!\n");
		return LJME_NO_DEVICES_FOUND;
	}
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		foundLabjacks[i]->syncDeviceTimes();
		foundLabjacks[i]->updateVisibleLightRelayIfNeeded();
	}


	// TODO - READ ME: main run loop
		// The LJM_StartInterval, LJM_WaitForNextInterval, and LJM_CleanInterval functions are used to efficiently execute the loop every so many milliseconds
		// To permit multiple labjacks operating in a general way, we probably want this loop to be contained within a thread that is owned by the BehavioralBoxLabjack object.
		// This main loop will simplely 
		// Each BehavioralBoxLabjack also needs to be responsible for writing out its own file.
		// There are some archetecture decisions to be made.
		// Perhaps turning the lights on and off should belong to the individual boxes as well.
		// Main should have perhaps an array of things?

	printCommandsMenu();
	printf("Collecting data at 20Hz....\n");
	//WaitForUserIfWindows();
	// Main run loop:
	int terminateExecution = 0;
	int character;
	do {
		/*
			Here we will perform the reading of the LabJack inputs (sensor values, etc).
		*/
		// Read a character from the keyboard
		character = _getch();
		character = toupper(character);
		if (character == 'Q') {
			cout << "Quitting..." << endl;
			terminateExecution = 1;
		}
		else if (character == 'S') {
			// Preview the current data
			cout << "Previewing current data..." << endl;

		}
		else if (character == 'P') {
			// Prints the current data
			cout << "Printing current data..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < foundLabjacks.size(); i++) {
				//time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
				//printf("runTopOfSecondUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
				foundLabjacks[i]->diagnosticPrintLastValues();
			}
		}
		else if (character == 'R') {
			cout << "Refreshing Labjacks..." << endl;
			int previouslyFoundLabjackSerialNumbers[max_number_labjacks] = {};
			int numberPreviouslyFoundLabjacks = foundLabjacks.size();
			for (int i = 0; i < numberPreviouslyFoundLabjacks; i++) {
				previouslyFoundLabjackSerialNumbers[i] = foundLabjacks[i]->getSerialNumber();
			}

			// Find the labjacks
			std::vector<BehavioralBoxLabjack*> newlyFoundAdditionalLabjacks = LabjackHelpers::findAllLabjacks(previouslyFoundLabjackSerialNumbers, numberPreviouslyFoundLabjacks);

			if (newlyFoundAdditionalLabjacks.size() > 0) {
				cout << "Found " << newlyFoundAdditionalLabjacks.size() << " new labjacks!" << endl;
				// Iterate through all newly found labjacks and append them to the list of found labjacks
				for (int i = 0; i < newlyFoundAdditionalLabjacks.size(); i++) {
					foundLabjacks.push_back(newlyFoundAdditionalLabjacks[i]);
				}
			}
			else {
				cout << "Found no new labjacks." << endl;
			}
		}
		else {
			printCommandsMenu();
		}

	} while (terminateExecution != 1);

	printf("Done.");

	return LJME_NOERROR;
}


// Print the list of options to execute
void printCommandsMenu() {
	cout << "Commands: " << endl;
	cout << "\t Press [p] at any time to print the most recently read values." << endl;
	cout << "\t Press [r] at any refresh and scan for more labjacks." << endl;
	cout << "\t Press [q] at any time to quit." << endl;
	cout << "\t Press any other key at any time to show this list of commands." << endl;
}

// Ran at the top of every hour
void runTopOfHourUpdate() {
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("runHourlyLightsUpdate: running at %s\n", ctime(&computerTime));
		// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfHourUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		foundLabjacks[i]->updateVisibleLightRelayIfNeeded();
	}
}

// Ran at the top of every minute
void runTopOfMinuteUpdate() {
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("runTopOfMinuteUpdate: running at %s\n", ctime(&computerTime));
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfMinuteUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		//foundLabjacks[i]->readSensorValues();
	}
	
}

// Ran at the top of every second
void runTopOfSecondUpdate() {
	time_t computerTime;
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfSecondUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		foundLabjacks[i]->readSensorValues();
	}
}

// Ran at a custom interval
void runPollingLoopUpdate() {
	//time_t computerTime;
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		//time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		//printf("runTopOfSecondUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		foundLabjacks[i]->readSensorValues();
	}
}


