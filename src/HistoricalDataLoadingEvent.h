#pragma once
#include <vector>
#include <functional>

#include "BehavioralBoxHistoricalData.h"
/*! \brief Encapsulate an event produced as a result of trying to load historical data.
 */
class HistoricalDataLoadingEvent
{
public:
	/*! \brief Enumeration for the event type.
	 */
	enum Type { ProgressUpdate, Complete };

	/*! \brief Get the event type.
	 */
	Type type() const { return type_; }

	/*! \brief Get the extra data for this event.
	 */
	const std::vector<BehavioralBoxHistoricalData>& dataLoadedHistoricalDataVector() const { return data_loadedHistoricalDataVector_; }
	const double& loadCompletionProgress() const { return loadCompletionProgress_; }

private:
	Type    type_;
	std::vector<BehavioralBoxHistoricalData> data_loadedHistoricalDataVector_;
	double loadCompletionProgress_ = 0.0;
	/*
	 * Data files update version
	 */
	HistoricalDataLoadingEvent(const std::vector<BehavioralBoxHistoricalData>& dataLoadedHistoricalDataVector)
		: type_(Complete), data_loadedHistoricalDataVector_(dataLoadedHistoricalDataVector), loadCompletionProgress_(100.0)
	{ }

	// Progress updated version
	HistoricalDataLoadingEvent(const double& updatedCompletionProgress) : type_(ProgressUpdate), data_loadedHistoricalDataVector_(std::vector<BehavioralBoxHistoricalData>()), loadCompletionProgress_(updatedCompletionProgress)
	{ }

	friend class BehavioralBoxControllersManager;
};

typedef std::function<void(const HistoricalDataLoadingEvent&)> HistoricalDataLoadingEventCallback;

