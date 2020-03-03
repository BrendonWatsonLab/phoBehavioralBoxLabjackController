#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

#include "FilesystemHelpers.h"
#include "BehavioralBoxLabjack.h"
#include "BehavioralBoxHistoricalData.h"
#include "HistoricalDataLoadingEvent.h"

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

	// Server Stuff:
	void connect(Client* client, const std::function<void()>& function);
	void disconnect(Client* client);

	std::vector<BehavioralBoxLabjack*> getActiveLabjacks() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return this->foundLabjacks_;
	}

	// Shutsdown the server
	void shutdown() {
		this->shouldStop_ = true;
	}

	// Scans for un-managed Labjacks:
	bool scanForNewLabjacks();

	// Idles and waits for a labjack to be found.
	bool waitForFoundLabjacks();
	//int shutdownApplication(int shutdownCode);

	std::vector<BehavioralBoxHistoricalData> getLoadedHistoricalData() { return this->historicalData_; }
	void reloadHistoricalData();

	// exports the historical data to individual .CSV files and returns the output paths.
	std::vector<std::string> exportHistoricalDataAsCSV(std::string path, std::string base_filename);

	// Server/Manager:
	//void postHistoricalDataUpdatedEvent(const HistoricalDataLoadingEvent& event);
	//serverGetAllHistoricalData(...): gets the latest version of the historical data
	void serverGetAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback);
	//serverLoadAllHistoricalData(...): reloads all historical data and then gets it
	void serverLoadAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback);

	// Getters:
	bool isReady();
	// Gets the computer's hostname
	std::string getHostName() { return this->hostName_; }
	// Gets the 2-digit integer identifier for the current computer (and box, if there is a 1-to-1 mapping). Like the "02" in "WATSON-BB-02"
	int getNumericComputerIdentifier() { return this->numeric_computer_identifer_; }
	


private:
	struct Connection {
		Connection(const std::string& id, Client* c,
			const std::function<void()>& f)
			: sessionId(id),
			client(c),
			function(f)
		{ }

		std::string sessionId;
		Client* client;
		std::function<void()> function;
	};

	// State Variables:
	mutable std::mutex mutex_;
	std::thread thread_;
	std::thread thread_reloadHistoricalData_; // used for reloading the historical data
	bool stillWaitingToFindLabjacks_ = true;
	bool shouldStop_ = false;



	// Vector of Labjack Objects
	std::vector<BehavioralBoxLabjack*> foundLabjacks_;
	int numberActiveLabjacks_ = 0;
	bool addLabjack(BehavioralBoxLabjack* newLabjack);
	
	// CSV Export testing variables
	const bool wants_export_to_csv_on_reload = true;
	std::string csvExportPath = "C:/Common/data/export-HistoricalData.csv";
	
	std::vector<Connection> connections_;

	void run();

	//Historical Labjack Data loading:
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	std::string hostName_ = configMan->getHostName();
	int numeric_computer_identifer_ = configMan->getNumericComputerIdentifier();
	std::string dataFilesSearchDirectory_ = configMan->getGeneratedActiveHistoricalSearchDirectory();

	// behavioralBoxEventDataFilesMap_: indexed by the BBID (1-indexed)
	std::map<int, std::vector<LabjackDataFile>> behavioralBoxEventDataFilesMap_;
	// TODO: replace labjackDataFilesMap_
	//std::map<int, std::vector<LabjackDataFile>> labjackDataFilesMap_;
	
	std::vector<BehavioralBoxHistoricalData> historicalData_;

	//TODO: currently doesn't make use of the individual Labjack object's output directory, and instead uses this class' member variable dataFilesSearchDirectory_
	//BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch);
	//BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber);
	
	
};



