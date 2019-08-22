#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "BehavioralBoxLajbackController.h"

BehavioralBoxLajbackController::BehavioralBoxLajbackController()
{
	this->thread_ = std::thread(std::bind(&BehavioralBoxLajbackController::run, this));
}

BehavioralBoxLajbackController::~BehavioralBoxLajbackController()
{
	this->shouldStop_ = true;
	this->thread_.join();
}

void BehavioralBoxLajbackController::connect(Client* client, const std::function<void()>& function)
{
	std::unique_lock<std::mutex> lock(mutex_);
	connections_.push_back(Connection(Wt::WApplication::instance()->sessionId(), client, function));
}

void BehavioralBoxLajbackController::disconnect(Client* client)
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

void BehavioralBoxLajbackController::run()
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
