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
#include <chrono>
#include <thread>

//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"
#include "BehavioralBoxLajbackController.h"
#include "BehavioralBoxLabjack.h"
#include "LabjackHelpers.h"

// Webserver functionality:
#if LAUNCH_WEB_SERVER
#include "WebServer.h"
#include "ChartsApplication.h"
#include <Wt/WServer.h>
// 1 Make the member a real variable not a pointer.
std::thread web_server_thread;
#endif // LAUNCH_WEB_SERVER

//BehavioralBoxLajbackController controller;

// Vector of Labjack Objects
std::vector<BehavioralBoxLabjack*> foundLabjacks;


//// Scheduler
#include "External/Scheduler/Scheduler.h"
// Make a new scheduling object.
  // Note: s cannot be moved or copied
Bosma::Scheduler s(max_n_threads);

// FUNCTION PROTOTYPES:
bool waitForFoundLabjacks();
#if LAUNCH_WEB_SERVER
bool startWebserver(int argc, char** argv);
#endif // LAUNCH_WEB_SERVER
int shutdownApplication(int shutdownCode);

// Interface:
void printCommandsMenu();


int main(int argc, char** argv)
{
	cout << "BehavioralBoxLabjackController:" << endl;
	cout << "\t Pho Hale 2019" << endl << endl;

#if LAUNCH_WEB_SERVER
	// Run the webserver:
	startWebserver(argc, argv);
#endif // LAUNCH_WEB_SERVER

	cout << endl << "Scanning for attached Labjacks..." << endl;
	if (!waitForFoundLabjacks()) {
		// User wants to quit.
		cout << "User chose to quit. Done." << endl;
		return shutdownApplication(LJME_NO_DEVICES_FOUND);
	}

#if LAUNCH_WEB_SERVER
	WServer::instance()->postAll(&ChartsApplication::staticUpdateActiveLabjacks);
#endif // LAUNCH_WEB_SERVER

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
			// Show the data files:
			cout << "Showing current log files..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < foundLabjacks.size(); i++) {
				std::string foundRelativeFilePathString = foundLabjacks[i]->getFullFilePath();
				std::string fullFilePathString = LabjackHelpers::getFullPath(foundRelativeFilePathString);

				cout << "\t Showing log file at " << fullFilePathString << endl;
				LabjackHelpers::showInExplorer(fullFilePathString);
			}
			cout << "\t done." << endl;
		}
		else if (character == 'P') {
			// Prints the current data
			cout << "Printing current data..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < foundLabjacks.size(); i++) {
				foundLabjacks[i]->diagnosticPrintLastValues();
			}
			cout << "\t done." << endl;
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
#if LAUNCH_WEB_SERVER
			// Refresh the webserver
			WServer::instance()->postAll(&ChartsApplication::staticUpdateActiveLabjacks);
#endif // LAUNCH_WEB_SERVER

			cout << "\t done." << endl;
		}
		else if (character == 'L') {
			cout << "Toggling visible LED Light mode on all labjacks..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < foundLabjacks.size(); i++) {
				foundLabjacks[i]->toggleOverrideMode_VisibleLED();
			}
			cout << "\t done." << endl;
		}
		else if (character == 'A') {
			cout << "Toggling attract mode on all Labjacks..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < foundLabjacks.size(); i++) {
				foundLabjacks[i]->toggleOverrideMode_AttractModeLEDs();
			}
			cout << "\t done." << endl;
		}
		else {
			printCommandsMenu();
		}

	} while (terminateExecution != 1);

	return shutdownApplication(LJME_NOERROR);
}

// Idles and waits for a labjack to be found.
bool waitForFoundLabjacks()
{
	bool stillWaitingToFindLabjacks = true;
	int character;
	do {
		// Find the labjacks
		foundLabjacks = LabjackHelpers::findAllLabjacks();

		if (foundLabjacks.size() == 0) {
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
			for (int i = 0; i < foundLabjacks.size(); i++) {
				foundLabjacks[i]->syncDeviceTimes();
				foundLabjacks[i]->updateVisibleLightRelayIfNeeded();
			}
			stillWaitingToFindLabjacks = false;
		}

	} while (stillWaitingToFindLabjacks == true);
	// Returns true to indicate that labjacks have been found and we should move forward with the program.
	return true;
}

#if LAUNCH_WEB_SERVER

bool startWebserver(int argc, char** argv)
{
	cout << "Starting the web server." << endl;
	web_server_thread = std::move(std::thread([=]() {
		chartsApplicationWebServer(argc, argv);
		return true;
	}));
	//runServer(argc, argv);
	return true;
}

#endif // LAUNCH_WEB_SERVER

// Called when the application is being quit
int shutdownApplication(int shutdownCode)
{
	cout << "Shutting down the application..." << endl;
	//controller.shutdown();
#if LAUNCH_WEB_SERVER
	cout << "Waiting on web server thread to quit..." << endl;
	// As the thread is using members from this object
	  // We can not let this obect be destroyed until
	  // the thread finishes executing.
	web_server_thread.join();
#endif // LAUNCH_WEB_SERVER
	printf("Done.");
	// At this point the thread has finished.
	// Destructor can now complete.
	return shutdownCode;
}


// Print the list of options to execute
void printCommandsMenu() {
	cout << "Commands: " << endl;
	cout << "\t Press [p] at any time to print the most recently read values for all labjacks." << endl;
	cout << "\t Press [s] at any time to show the current log files for all labjacks." << endl;
	cout << "\t Press [r] at any time to refresh and scan for more labjacks." << endl;
	cout << "\t Press [l] at any time to toggle visible LED Light mode for all labjacks." << endl;
	cout << "\t Press [a] at any time to toggle Attract mode for all labjacks." << endl;
	cout << "\t Press [q] at any time to quit." << endl;
	cout << "\t Press any other key at any time to show this list of commands." << endl;
}

