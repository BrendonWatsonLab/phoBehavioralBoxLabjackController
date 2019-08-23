#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

#include "BehavioralBoxLabjack.h"
#include "BehavioralBoxHistoricalData.h"

//TODO: perhaps this should be referred to as a "Manager" of controllers.


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

	//bool waitForFoundLabjacks();
	//int shutdownApplication(int shutdownCode);

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

	mutable std::mutex mutex_;
	std::thread thread_;
	// Vector of Labjack Objects
	std::vector<BehavioralBoxLabjack*> foundLabjacks_;
	bool shouldStop_ = false;

	std::vector<Connection> connections_;

	void run();

	//Historical Labjack Data loading:
	std::vector<BehavioralBoxHistoricalData> historicalData_;
	void loadHistoricalData();
	
};



