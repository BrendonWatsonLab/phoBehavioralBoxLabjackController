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
#if INCLUDE_WEB_SERVER_FILES
#include "LabjackControllerWebApplication.h"

#include <Wt/WServer.h>
// 1 Make the member a real variable not a pointer.
std::thread web_server_thread;
#endif // LAUNCH_WEB_SERVER

//BehavioralBoxControllersManager controller;
std::shared_ptr<BehavioralBoxControllersManager> controller = std::make_shared<BehavioralBoxControllersManager>();



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
	std::cout << "BehavioralBoxLabjackController:" << std::endl;
	std::cout << "\t Version " << SOFTWARE_VERSION << std::endl;
	std::cout << "\t Pho Hale 2021" << std::endl << std::endl;
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();

	configMan->testJsonConfig();
	
	// Get the hostname
	//std::string foundHostName = configMan->getHostName();
	//std::cout << "Found host name: " << foundHostName << std::endl;
	//int computerIdentifierNumber = configMan->getNumericComputerIdentifier();
	//std::cout << "Computer Identifier ID: " << computerIdentifierNumber << std::endl;

	printConfiguration();

	//TODO: this doesn't currently matter because the webserver reloads everything in TimeSeriesChart::buildHistoricDataModel() by calling the static BehavioralBoxControllersManager::loadAllHistoricalData() function.
	// Eventually we weant to implement it in a singleton-like fashion.

	#if LAUNCH_WEB_SERVER
		const bool shouldStartWebServer = configMan->getLoadedConfig().launch_web_server;

		if (shouldStartWebServer) {
			// Run the webserver:
			startWebserver(argc, argv, &controller);
		}

	
	#else

	
	#endif
	

	std::cout <<std::endl << "Scanning for attached Labjacks..." <<std::endl;
	if (!controller->waitForFoundLabjacks()) {
		// User wants to quit.
		std::cout << "User chose to quit. Done." <<std::endl;
		return shutdownApplication(LJME_NO_DEVICES_FOUND);
	}

#if LAUNCH_WEB_SERVER
	if (shouldStartWebServer) {
		Wt::WServer::instance()->postAll(&LabjackControllerWebApplication::staticUpdateActiveLabjacks);
	}
#endif

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
		std::cout << "Collecting data at " << (1000.0 / double(LABJACK_UPDATE_LOOP_FREQUENCY_MILLISEC)) << "Hz...." <<std::endl;
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
			std::cout << "Quitting..." << std::endl;
			terminateExecution = 1;
		}
		else if (character == 'S') {
			// Show the data files:
			std::cout << "Showing current log files..." << std::endl;
			// Iterate through all found Labjacks
			
			for (int i = 0; i < controller->getActiveLabjacks().size(); i++) {
				std::string foundRelativeFilePathString = controller->getActiveLabjacks()[i]->getFullFilePath();
				std::string fullFilePathString = LabjackHelpers::getFullPath(foundRelativeFilePathString);

				std::cout << "\t Showing log file at " << fullFilePathString << std::endl;
				LabjackHelpers::showInExplorer(fullFilePathString);
			}
			std::cout << "\t done." << std::endl;
		}
		else {
			printCommandsMenu();
		}

	} while (terminateExecution != 1);

	return shutdownApplication(LJME_NOERROR);
}


bool startWebserver(int argc, char** argv, const std::shared_ptr<BehavioralBoxControllersManager>* managerPtr)
{
#if INCLUDE_WEB_SERVER_FILES
	std::cout << "Starting the web server." << std::endl;

	// WEB SERVER THREAD BLOCK:
	web_server_thread = std::move(std::thread([=]() {
		// Build the input arguments for the webserver
		// Converted using https://stackoverflow.com/questions/26032039/convert-vectorstring-into-char-c
		// Only works within the thread's block, when placed before it, it failed.
		//std::vector<std::string> strings{ "C:\\Common\\repo\\phoBehavioralBoxLabjackController\\x64\\Release\\phoBehavioralBoxLabjackController.exe", "--docroot", ".", "--config", "C:\\Common\\repo\\phoBehavioralBoxLabjackController\\/ConfigFiles/wt_config.xml", "--http-address", "0.0.0.0", "--http-port", "8080", "-accesslog=C:\\Common\\repo\\phoBehavioralBoxLabjackController\\/logs/webServerAccessLog.log" };
		std::vector<std::string> stringsWebserverArguments{ argv[0], "--docroot", ".", "--config", "C:/Common/config/phoBehavioralBoxLabjackController-WT_config.xml", "--http-address", "0.0.0.0", "--http-port", "8080", "-accesslog=C:/Common/info/webServerAccessLog.log" };

		std::vector<char*> cstringsWebserverArguments{};

		for (auto& currStr : stringsWebserverArguments) {
			cstringsWebserverArguments.push_back(&currStr.front());
		}

		char** finalArgs = (char**)cstringsWebserverArguments.data();
		labjackControllerApplicationWebServer(cstringsWebserverArguments.size(), finalArgs, managerPtr);
		
		//WServer::instance()->postAll(&LabjackControllerWebApplication::staticUpdateActiveLabjacks);

		return true;
	}));

#endif
	return true;
}

// Called when the application is being quit
int shutdownApplication(int shutdownCode)
{
	std::cout << "Shutting down the application..." << std::endl;
	//controller->shutdown();
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
#if INCLUDE_WEB_SERVER_FILES
	const bool shouldStartWebServer = configMan->getLoadedConfig().launch_web_server;
	if (shouldStartWebServer) {
		std::cout << "Waiting on web server thread to quit..." << std::endl;
		// As the thread is using members from this object
		// We can not let this object be destroyed until the thread finishes executing.
		web_server_thread.join();
	}
#endif
	printf("Done.");
	// At this point the thread has finished.
	// Destructor can now complete.
	return shutdownCode;
}


// Print the list of options to execute
void printCommandsMenu() {
	std::cout << "Commands: " << std::endl;
	std::cout << "\t Press [p] at any time to print the most recently read values for all labjacks." << std::endl;
	std::cout << "\t Press [f] at any time to show the current output file directory." << std::endl;
	std::cout << "\t Press [s] at any time to show the current log files for all labjacks." << std::endl;
	/*std::cout << "\t Press [r] at any time to refresh and scan for more labjacks." << std::endl;
	std::cout << "\t Press [l] at any time to toggle visible LED Light mode for all labjacks." << std::endl;
	std::cout << "\t Press [a] at any time to toggle Attract mode for all labjacks." <<std::endl;
	std::cout << "\t Press [u] at any time to display utility options." <<std::endl;*/
	std::cout << "\t Press [q] at any time to quit." <<std::endl;
	std::cout << "\t Press any other key at any time to show this list of commands." <<std::endl;
}


void printFilesystemPaths() {
	std::cout << "Filesystem: " <<std::endl;
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
	auto loaded_config = configMan->getLoadedConfig();
	std::cout << "\t Output Directory: " << configMan->getGeneratedActiveOutputDirectory() <<std::endl;
	if (loaded_config.enableHistoricalDataLoading) {
		std::cout << "\t Historical data loading is enabled and the output directory is " << configMan->getGeneratedActiveHistoricalSearchDirectory() <<std::endl;
	}
	else {
		std::cout << "\t Historical data loading is disabled." <<std::endl;
	}
}

void printComputerInformation() {
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
	std::cout << "Computer Hostname: " << configMan->getHostName() <<std::endl;
}

void printConfiguration() {
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
	auto loaded_config = configMan->getLoadedConfig();
	std::cout << "CURRENT CONFIGURATION: ==============================" <<std::endl;
	printComputerInformation();
	std::cout << "\t Experiment Name: " << loaded_config.experimentName <<std::endl;
	std::cout << "\t Cohort Name: " << loaded_config.cohortName <<std::endl;
	std::cout << "\t Animal Name: " << loaded_config.animalName <<std::endl;
	printFilesystemPaths();
	std::cout << "Webserver is ";
	if (loaded_config.launch_web_server) {
		std::cout << "enabled. It can be accessed via a web browser at URL http://127.0.0.1:8080" <<std::endl;
	}
	else {
		std::cout << "disabled." <<std::endl;
	}
	std::cout << "==========================  END CURRENT CONFIGURATION" <<std::endl;
}


