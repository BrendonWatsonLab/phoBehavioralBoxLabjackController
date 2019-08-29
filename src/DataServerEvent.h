#pragma once
#include <map>
#include <vector>
#include <functional>

#include "LabjackDataFile.h"

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