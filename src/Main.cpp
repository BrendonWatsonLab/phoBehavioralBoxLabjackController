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
#include "BehavioralBoxControllersManager.h"
#include "BehavioralBoxLabjack.h"
#include "LabjackHelpers.h"
#include "ConfigurationManager.h"

std::shared_ptr<BehavioralBoxControllersManager> controller = make_shared<BehavioralBoxControllersManager>();


// FUNCTION PROTOTYPES:
//bool waitForFoundLabjacks();

bool startWebserver(int argc, char** argv, const std::shared_ptr<BehavioralBoxControllersManager>* managerPtr);

int shutdownApplication(int shutdownCode);

// Interface:
void printComputerInformation();
void printFilesystemPaths();
void printConfiguration();
void printCommandsMenu();


int main(int argc, char** argv)
{
	cout << "BehavioralBoxLabjackController:" << endl;
	cout << "\t Version " << SOFTWARE_VERSION << endl;
	cout << "\t Pho Hale 2019" << endl << endl;
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();

	// Get the hostname
	//std::string foundHostName = configMan->getHostName();
	//cout << "Found host name: " << foundHostName << endl;
	//int computerIdentifierNumber = configMan->getNumericComputerIdentifier();
	//cout << "Computer Identifier ID: " << computerIdentifierNumber << endl;

	printConfiguration();

	cout << endl << "Scanning for attached Labjacks..." << endl;
	if (!controller->waitForFoundLabjacks()) {
		// User wants to quit.
		cout << "User chose to quit. Done." << endl;
		return shutdownApplication(LJME_NO_DEVICES_FOUND);
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
	if (controller->getActiveLabjacks().size() > 0) {
		cout << "Collecting data at " << (1000.0 / double(LABJACK_UPDATE_LOOP_FREQUENCY_MILLISEC)) << "Hz...." << endl;
	}
	
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
			
			for (int i = 0; i < controller->getActiveLabjacks().size(); i++) {
				std::string foundRelativeFilePathString = controller->getActiveLabjacks()[i]->getFullFilePath();
				std::string fullFilePathString = LabjackHelpers::getFullPath(foundRelativeFilePathString);

				cout << "\t Showing log file at " << fullFilePathString << endl;
				LabjackHelpers::showInExplorer(fullFilePathString);
			}
			cout << "\t done." << endl;
		}
		else if (character == 'F') {
			// Show the data files:
			cout << "Showing current output directory..." << endl;
			// Iterate through all found Labjacks
			auto loaded_config = configMan->getLoadedConfig();
			std::string fullOutputDirectoryPathString = configMan->getGeneratedActiveOutputDirectory();
			cout << "\t Showing output file directory at " << fullOutputDirectoryPathString << endl;
			LabjackHelpers::showInExplorer(fullOutputDirectoryPathString);
			cout << "\t done." << endl;
		}
		else if (character == 'P') {
			// Prints the current data
			cout << "Printing current data..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < controller->getActiveLabjacks().size(); i++) {
				controller->getActiveLabjacks()[i]->diagnosticPrintLastValues();
			}
			cout << "\t done." << endl;
		}
		else if (character == 'R') {
			cout << "Refreshing Labjacks..." << endl;
			controller->scanForNewLabjacks();
			cout << "\t done." << endl;
		}
		else if (character == 'L') {
			cout << "Toggling visible LED Light mode on all labjacks..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < controller->getActiveLabjacks().size(); i++) {
				controller->getActiveLabjacks()[i]->toggleOverrideMode_VisibleLED();
			}
			cout << "\t done." << endl;
		}
		else if (character == 'A') {
			cout << "Toggling attract mode on all Labjacks..." << endl;
			// Iterate through all found Labjacks
			for (int i = 0; i < controller->getActiveLabjacks().size(); i++) {
				controller->getActiveLabjacks()[i]->toggleOverrideMode_AttractModeLEDs();
			}
			cout << "\t done." << endl;
		}
		else if (character == 'U') {
			cout << "Utility mode:" << endl;
			//TODO: utility mode.
			// Export data as CSV
			std::vector<std::string> exportPaths = controller->exportHistoricalDataAsCSV("C:/Common/data/", "export-HistoricalData");
			if (exportPaths.empty()) {
				cout << "Tried to export CSV files but had no historical data." << endl;
			}
			else {
				int numExportPaths = exportPaths.size();
				cout << "Export " << numExportPaths << " historical data .CSV files:" << endl;
				for (size_t i = 0; i < numExportPaths; i++)
				{
					cout << "\t" << exportPaths[i] << endl;
				}
			}
			cout << "\t done." << endl;
		}
		else {
			printCommandsMenu();
		}

	} while (terminateExecution != 1);

	return shutdownApplication(LJME_NOERROR);
}


// Called when the application is being quit
int shutdownApplication(int shutdownCode)
{
	cout << "Shutting down the application..." << endl;
	//controller->shutdown();
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	printf("Done.");
	// At this point the thread has finished.
	// Destructor can now complete.
	return shutdownCode;
}


// Print the list of options to execute
void printCommandsMenu() {
	cout << "Commands: " << endl;
	cout << "\t Press [p] at any time to print the most recently read values for all labjacks." << endl;
	cout << "\t Press [f] at any time to show the current output file directory." << endl;
	cout << "\t Press [s] at any time to show the current log files for all labjacks." << endl;
	cout << "\t Press [r] at any time to refresh and scan for more labjacks." << endl;
	cout << "\t Press [l] at any time to toggle visible LED Light mode for all labjacks." << endl;
	cout << "\t Press [a] at any time to toggle Attract mode for all labjacks." << endl;
	cout << "\t Press [u] at any time to display utility options." << endl;
	cout << "\t Press [q] at any time to quit." << endl;
	cout << "\t Press any other key at any time to show this list of commands." << endl;
}


void printFilesystemPaths() {
	cout << "Filesystem: " << endl;
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	auto loaded_config = configMan->getLoadedConfig();
	cout << "\t Output Directory: " << configMan->getGeneratedActiveOutputDirectory() << endl;
	if (loaded_config.enableHistoricalDataLoading) {
		cout << "\t Historical data loading is enabled and the output directory is " << configMan->getGeneratedActiveHistoricalSearchDirectory() << endl;
	}
	else {
		cout << "\t Historical data loading is disabled." << endl;
	}
}

void printComputerInformation() {
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	cout << "Computer Hostname: " << configMan->getHostName() << endl;

	auto numeric_client_identifier = configMan->getNumericComputerIdentifier();
	if (numeric_client_identifier == -1)
	{
		cout << "WARNING: Client's BB Identifier Number could not be found. Is this software running on a Behavioral Box Client computer?" << endl;
	}
	else {
		// Computer is a BB Client
		cout << "Parsed BB Identifier Number: " << numeric_client_identifier << endl;
	}
	// configMan->getEnvironmentVariables();
}

void printConfiguration() {
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	auto loaded_config = configMan->getLoadedConfig();
	cout << "CURRENT CONFIGURATION: ==============================" << endl;
	printComputerInformation();
	cout << "\t Experiment Name: " << loaded_config.experimentName << endl;
	cout << "\t Cohort Name: " << loaded_config.cohortName << endl;
	cout << "\t Animal Name: " << loaded_config.animalName << endl;
	printFilesystemPaths();
	cout << "==========================  END CURRENT CONFIGURATION" << endl;
}


