#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

#include "FilesystemHelpers.h"
#include "BehavioralBoxLabjack.h"
//#include "BehavioralBoxHistoricalData.h"
//#include "HistoricalDataLoadingEvent.h"

#include "ConfigurationManager.h"


/*
 * Simple interface to uniquely identify a client
 */
class Client {
};


 /*! \brief  A (singleton) server class which would protect and manage a shared resource of a vector of Labjacks.
  * Manages Labjacks, found data files, and historical data
  */
class BehavioralBoxControllersManager
{
public:
	BehavioralBoxControllersManager();
	~BehavioralBoxControllersManager();

	std::vector<BehavioralBoxLabjack*> getActiveLabjacks() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return this->foundLabjacks_;
	}

	// Scans for un-managed Labjacks:
	bool scanForNewLabjacks();

	// Idles and waits for a labjack to be found.
	bool waitForFoundLabjacks();
	//int shutdownApplication(int shutdownCode);

	// Getters:
	bool isReady();
	// Gets the computer's hostname
	std::string getHostName() { return this->hostName_; }
	// Gets the 2-digit integer identifier for the current computer (and box, if there is a 1-to-1 mapping). Like the "02" in "WATSON-BB-02"
	int getNumericComputerIdentifier() { return this->numeric_computer_identifer_; }
	
private:

	// State Variables:
	mutable std::mutex mutex_;
	std::thread thread_;
	bool stillWaitingToFindLabjacks_ = false;
	bool shouldStop_ = false;



	// Vector of Labjack Objects
	std::vector<BehavioralBoxLabjack*> foundLabjacks_;
	int numberActiveLabjacks_ = 0;
	bool addLabjack(BehavioralBoxLabjack* newLabjack);
	
	// CSV Export testing variables
	const bool wants_export_to_csv_on_reload = false;
	std::string csvExportPath = "C:/Common/data/export-HistoricalData.csv";

	void run();

	// behavioralBoxEventDataFilesMap_: indexed by the BBID (1-indexed)
	std::map<int, std::vector<LabjackDataFile>> behavioralBoxEventDataFilesMap_;
	// TODO: replace labjackDataFilesMap_
	//std::map<int, std::vector<LabjackDataFile>> labjackDataFilesMap_;
	
	//TODO: currently doesn't make use of the individual Labjack object's output directory, and instead uses this class' member variable dataFilesSearchDirectory_
	//BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch);
	//BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber);
	
	
};



