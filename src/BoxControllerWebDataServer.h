#pragma once
#include <Wt/WSignal.h>
#include <Wt/WString.h>

#include <set>
#include <map>
#include <thread>
#include <mutex>

#include "LabjackDataFile.h"
#include "DataServerEvent.h"

//class LabjackDataFile;
#include "BehavioralBoxControllersManager.h"


namespace Wt {
	class WServer;
}

//BoxControllerWebDataServer
class BoxControllerWebDataServer
{
public:
	/*
	 * A reference to a client.
	 */
	class Client
	{
	};

	/*! \brief Create a new chat server.
	 */
	BoxControllerWebDataServer(Wt::WServer& server);

	BoxControllerWebDataServer(const BoxControllerWebDataServer&) = delete;
	BoxControllerWebDataServer& operator=(const BoxControllerWebDataServer&) = delete;

	/*! \brief Connects to the data server.
	 *
	 * The passed callback method is posted to when a new data server event is
	 * received.
	 *
	 * Returns whether the client has been connected (or false if the client
	 * was already connected).
	 */
	bool connect(Client* client, const DataServerEventCallback& handleEvent);

	/*! \brief Disconnect from the chat server.
	 *
	 * Returns whether the client has been disconnected (or false if the client
	 * was not connected).
	 */
	bool disconnect(Client* client);

	///*! \brief Send a message on behalve of a user.
	// */
	//void sendMessage(const Wt::WString& user, const Wt::WString& message);

	void processHistoricalDataUpdateEvent(const HistoricalDataLoadingEvent& event);


private:
	struct ClientInfo {
		std::string       sessionId;
		DataServerEventCallback eventCallback;
	};

	typedef std::map<Client*, ClientInfo> ClientMap;

	Wt::WServer& server_;
	std::recursive_mutex    mutex_;
	ClientMap               clients_;

	BehavioralBoxControllersManager& manager_;

	void postDataServerEvent(const DataServerEvent& event);
};

