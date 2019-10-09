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

// Webserver functionality:
#if LAUNCH_WEB_SERVER
#include "LabjackControllerWebApplication.h"

#include <Wt/WServer.h>
// 1 Make the member a real variable not a pointer.
std::thread web_server_thread;
#endif // LAUNCH_WEB_SERVER

//BehavioralBoxControllersManager controller;
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
	cout << "\t Pho Hale 2019" << endl << endl;
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();

	// Get the hostname
	//std::string foundHostName = configMan->getHostName();
	//cout << "Found host name: " << foundHostName << endl;
	//int computerIdentifierNumber = configMan->getNumericComputerIdentifier();
	//cout << "Computer Identifier ID: " << computerIdentifierNumber << endl;

	printConfiguration();

	//TODO: this doesn't currently matter because the webserver reloads everything in TimeSeriesChart::buildHistoricDataModel() by calling the static BehavioralBoxControllersManager::loadAllHistoricalData() function.
	// Eventually we weant to implement it in a singleton-like fashion.
	const bool shouldStartWebServer = configMan->getLoadedConfig().launch_web_server;
	if (shouldStartWebServer) {
		// Run the webserver:
		startWebserver(argc, argv, &controller);
	}

	cout << endl << "Scanning for attached Labjacks..." << endl;
	if (!controller->waitForFoundLabjacks()) {
		// User wants to quit.
		cout << "User chose to quit. Done." << endl;
		return shutdownApplication(LJME_NO_DEVICES_FOUND);
	}

	if (shouldStartWebServer) {
		WServer::instance()->postAll(&LabjackControllerWebApplication::staticUpdateActiveLabjacks);
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
			if (shouldStartWebServer) {
				// Refresh the webserver
				WServer::instance()->postAll(&LabjackControllerWebApplication::staticUpdateActiveLabjacks);
			}
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


bool startWebserver(int argc, char** argv, const std::shared_ptr<BehavioralBoxControllersManager>* managerPtr)
{

	cout << "Starting the web server." << endl;
	//// Debug only, output the input args
	//cout << "Input arguments: ";
	//cout << "{";
	//for (size_t i = 0; i < argc; i++)
	//{
	//	std::string currArgument = argv[i];
	//	cout << "\"" << currArgument << "\", ";
	//}
	//cout << "}" << endl;
	//cout << "end input arguments.";

	web_server_thread = std::move(std::thread([=]() {
		// Build the input arguments for the webserver
		// Converted using https://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
		// Only works within the thread's block, when placed before it, it failed.
//std::vector<std::string> strings{ "C:\\Common\\repo\\phoBehavioralBoxLabjackController\\x64\\Release\\phoBehavioralBoxLabjackController.exe", "--docroot", ".", "--config", "C:\\Common\\repo\\phoBehavioralBoxLabjackController\\/ConfigFiles/wt_config.xml", "--http-address", "0.0.0.0", "--http-port", "8080", "-accesslog=C:\\Common\\repo\\phoBehavioralBoxLabjackController\\/logs/webServerAccessLog.log" };
		std::vector<std::string> stringsWebserverArguments{ argv[0], "--docroot", ".", "--config", "C:/Common/config/phoBehavioralBoxLabjackController-WT_config.xml", "--http-address", "0.0.0.0", "--http-port", "8080", "-accesslog=C:/Common/info/webServerAccessLog.log" };

		std::vector<char*> cstringsWebserverArguments{};
		//cstringsWebserverArguments.reserve(stringsWebserverArguments.size());

		for (auto& currStr : stringsWebserverArguments) {
			cstringsWebserverArguments.push_back(&currStr.front());
		}

		/*std::vector<const char*> cstringsWebserverArguments{};

		for (const auto& currStr : stringsWebserverArguments) {
			cstringsWebserverArguments.push_back(currStr.c_str());
		}*/
		char** finalArgs = (char**)cstringsWebserverArguments.data();
		labjackControllerApplicationWebServer(cstringsWebserverArguments.size(), finalArgs, managerPtr);
		
		return true;
	}));
	//runServer(argc, argv);
	return true;
}

// Called when the application is being quit
int shutdownApplication(int shutdownCode)
{
	cout << "Shutting down the application..." << endl;
	//controller->shutdown();
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	const bool shouldStartWebServer = configMan->getLoadedConfig().launch_web_server;
	if (shouldStartWebServer) {
		cout << "Waiting on web server thread to quit..." << endl;
		// As the thread is using members from this object
		// We can not let this object be destroyed until the thread finishes executing.
		web_server_thread.join();
	}
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
	cout << "Parsed BB Identifier Number: " << configMan->getNumericComputerIdentifier() << endl;
	configMan->getEnvironmentVariables();
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
	cout << "Webserver is ";
	if (loaded_config.launch_web_server) {
		cout << "enabled. It can be accessed via a web browser at URL http://127.0.0.1:8080" << endl;
	}
	else {
		cout << "disabled." << endl;
	}
	cout << "==========================  END CURRENT CONFIGURATION" << endl;
}


