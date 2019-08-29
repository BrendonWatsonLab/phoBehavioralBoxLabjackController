#pragma once
#include <Wt/WSignal.h>
#include <Wt/WString.h>

#include <set>
#include <map>
#include <thread>
#include <mutex>

#include "LabjackDataFile.h"
//class LabjackDataFile;


namespace Wt {
	class WServer;
}

/*! \brief Encapsulate a chat event.
 */
class DataServerEvent
{
public:
	/*! \brief Enumeration for the event type.
	 */
	enum Type { Login, Logout, LabjackListRefreshed, HistoricalDataRefreshed, LabjackLiveDataRefreshed };

	/*! \brief Get the event type.
	 */
	Type type() const { return type_; }

	/*! \brief Get the extra data for this event.
	 */
	const std::map<int, std::vector<LabjackDataFile>>& dataFilesMap() const { return data_labjackDataFilesMap_; }
	//const Wt::WString& data() const { return data_; }


private:
	Type    type_;
	//Wt::WString user_;
	// Data Block
	std::map<int, std::vector<LabjackDataFile>> data_labjackDataFilesMap_;
	//Wt::WString data_;
	//Wt::WString message_;

	/*
	 * Data files update version
	 */
	DataServerEvent(const std::map<int, std::vector<LabjackDataFile>>& dataFilesMap)
		: type_(HistoricalDataRefreshed), data_labjackDataFilesMap_(dataFilesMap)
	{ }

	// Other version
	DataServerEvent(Type eventType)
		: type_(eventType), data_labjackDataFilesMap_(std::map<int, std::vector<LabjackDataFile>>())
	{ }

	//TODO: add a labjacks vector and make an initialzier for that version too.

	friend class BoxControllerWebDataServer;
};

typedef std::function<void(const DataServerEvent&)> DataServerEventCallback;



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


private:
	struct ClientInfo {
		std::string       sessionId;
		DataServerEventCallback eventCallback;
	};

	typedef std::map<Client*, ClientInfo> ClientMap;

	Wt::WServer& server_;
	std::recursive_mutex    mutex_;
	ClientMap               clients_;

	void postDataServerEvent(const DataServerEvent& event);
};

