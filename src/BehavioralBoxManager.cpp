#include <limits>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "BehavioralBoxControllersManager.h"

BehavioralBoxManager::BehavioralBoxManager()
{
	this->thread_ = std::thread(std::bind(&BehavioralBoxManager::run, this));
}

BehavioralBoxManager::~BehavioralBoxManager()
{
	this->shouldStop_ = true;
	this->thread_.join();
}

void BehavioralBoxManager::connect(Client* client, const std::function<void()>& function)
{
	std::unique_lock<std::mutex> lock(mutex_);
	connections_.push_back(Connection(Wt::WApplication::instance()->sessionId(), client, function));
}

void BehavioralBoxManager::disconnect(Client* client)
{
	std::unique_lock<std::mutex> lock(mutex_);

	for (unsigned i = 0; i < connections_.size(); ++i) {
		if (connections_[i].client == client) {
			connections_.erase(connections_.begin() + i);
			return;
		}
	}
	assert(false);
}

void BehavioralBoxManager::run()
{
	/*
   * This method simulates changes to the data that happen in a background
   * thread.
   */
	for (;;) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (this->shouldStop_) {
			return;
		}

		{
			std::unique_lock<std::mutex> lock(mutex_);
			//++counter_;
			//TODO: Update the number of labjacks and such
			// **HERE**


			// **HERE**
			/* This is where we notify all connected clients. */
			for (unsigned i = 0; i < connections_.size(); ++i) {
				Connection& c = connections_[i];
				Wt::WServer::instance()->post(c.sessionId, c.function);
			}
		}
	}
}

void BehavioralBoxManager::loadHistoricalData()
{
	this->historicalData_.clear();
	for (int i = 0; i < this->getActiveLabjacks().size(); i++) {
		
		//std::vector<LabjackDataFile> currLabjackDataFile = this->findDataFiles(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
		BehavioralBoxHistoricalData currHistoryManager = BehavioralBoxHistoricalData(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
		this->historicalData_.push_back(currHistoryManager);
	}
}
