#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

#include "FilesystemHelpers.h"
#include "BehavioralBoxLabjack.h"
#include "BehavioralBoxHistoricalData.h"
#include "HistoricalDataLoadingEvent.h"


/*
 * Simple interface to uniquely identify a client
 */
class Client {
};

/*
 * A (singleton) server class which would protect and manage a shared
 * resource of a vector of Labjacks. In our example we take a simple counter as data.
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


	// Server/Manager:
	//void postHistoricalDataUpdatedEvent(const HistoricalDataLoadingEvent& event);
	//serverGetAllHistoricalData(...): gets the latest version of the historical data
	void serverGetAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback);
	//serverLoadAllHistoricalData(...): reloads all historical data and then gets it
	void serverLoadAllHistoricalData(HistoricalDataLoadingEventCallback completionCallback);
	static std::vector<BehavioralBoxHistoricalData> loadAllHistoricalData();

	// Getters:
	bool isReady();

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
	bool stillWaitingToFindLabjacks_ = true;
	bool shouldStop_ = false;



	// Vector of Labjack Objects
	std::vector<BehavioralBoxLabjack*> foundLabjacks_;
	int numberActiveLabjacks_ = 0;
	bool addLabjack(BehavioralBoxLabjack* newLabjack);
	
	
	std::vector<Connection> connections_;

	void run();

	//Historical Labjack Data loading:
	std::string dataFilesSearchDirectory_ = "output_data/";
	std::map<int, std::vector<LabjackDataFile>> labjackDataFilesMap_;
	std::vector<BehavioralBoxHistoricalData> historicalData_;

	//TODO: currently doesn't make use of the individual Labjack object's output directory, and instead uses this class' member variable dataFilesSearchDirectory_
	BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch);
	BehavioralBoxHistoricalData getHistoricalData(int labjackSerialNumber);
	
	
};



