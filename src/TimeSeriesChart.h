#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WAbstractItemModel.h>

#include "BoxPortInformation.h"

#define TIME_SERIES_CHART_ENABLE_AGGREGATE_STATS true
#define TIME_SERIES_CHART_NUM_TABLE_ROWS_SHOWN 8
#define TIME_SERIES_CHART_NUM_TABLE_ROW_HEIGHT 20
#define TIME_SERIES_CHART_SUBPLOT_HEIGHT 130
//#define TIME_SERIES_CHART_SUBPLOT_HEIGHT 50
#define TIME_SERIES_CHART_SUBPLOT_WIDTH 1080
#define TIME_SERIES_CHART_RANGE_SLIDER_HEIGHT 80

class BehavioralBoxHistoricalData;
class HistoricalDataLoadingEvent;

/* Config:
shouldEnableSynchronize_Y_Axis
numDaysToDisplay
*/

/*
* Shows raw "event" data as well as "aggregate" (summed) data.
*/
class TimeSeriesChart : public Wt::WContainerWidget
{
public:
	TimeSeriesChart();

	struct DataDisplayOptions {
		bool shouldShowEventData = false;
		bool shouldShowAggregateData = false;
		DataDisplayOptions(bool showEvents, bool showAggregate) : shouldShowEventData(showEvents), shouldShowAggregateData(showAggregate) {};
	};

	struct TimeSeriesChartDisplayOptions {
		bool shouldShowTable = false;
		bool shouldShowPlot = false;

		//bool shouldShowIndividualEvents = false;
		//bool shouldEnableAggregateEvents = TIME_SERIES_CHART_ENABLE_AGGREGATE_STATS;

		TimeSeriesChartDisplayOptions(bool showTable, bool showPlot) : shouldShowTable(showTable), shouldShowPlot(showPlot) {};
	};


	TimeSeriesChartDisplayOptions totalDisplayOptions = TimeSeriesChartDisplayOptions(false, true);
	DataDisplayOptions tableDisplayOptions = DataDisplayOptions(false, false);
	//DataDisplayOptions plotDisplayOptions = DataDisplayOptions(true, true);
	DataDisplayOptions plotDisplayOptions = DataDisplayOptions(false, true);

	std::vector<Wt::WColor> getVariableColors() { return this->colorVect_; }
	Wt::WColor getDefaultColor() { return this->otherColor_; }

	std::shared_ptr<Wt::WAbstractItemModel> model;

	// Update Function:
	void reload(BehavioralBoxHistoricalData historicalData);
	//void processHistoricalDataUpdateEvent(const HistoricalDataLoadingEvent& event);

	struct CurrentDateTimeRange {

		unsigned long long start;
		unsigned long long end;
		CurrentDateTimeRange(unsigned long long start, unsigned long long end) : start(start), end(end) {};

	};

	CurrentDateTimeRange getCurrentDesiredRangeMillis();

	// Axis Synchronization:
	double shared_y_axis_max = 0.0;
	double shared_y_axis_min = 0.0;
	// shouldEnableSynchronize_Y_Axis: if true, the Y-Axis scale is sychronized across all subplots.
	bool shouldEnableSynchronize_Y_Axis = false;


private:
	// Config:
	bool shouldEnableAggregateStatistics_ = TIME_SERIES_CHART_ENABLE_AGGREGATE_STATS;
	int numDaysToDisplay_ = 60;
	bool shouldUseDateXAxis = true;

	// UI Elements
	WContainerWidget* loadingContainerWidget;
	WContainerWidget* tableContainerWidget;
	WContainerWidget* chartsContainerWidget;

	// GUI Setup Functions:
	void setupLoadingIndicator();
	void setupTable(const std::shared_ptr<Wt::WAbstractItemModel> model);
	void setupCharts(const std::shared_ptr<Wt::WAbstractItemModel> model);

	bool isLoadingIndicatorVisible = true;
	void changeLoadingIndicatorVisibility(bool shouldLoadingIndicatorBeVisible);

	std::shared_ptr<Wt::WStandardItemModel> buildHistoricDataModel(BehavioralBoxHistoricalData activeHistoricalData);
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> buildDataSeries(const std::shared_ptr<Wt::WAbstractItemModel> model);

	// Data Series:
	const int getNumSubplots() { return this->subplotDataSeriesIndicies_.size(); }; // 4 food/water signals + 1 for running wheel
	const int getNumVariables() { return this->colorVect_.size(); };

	void loadFromConfig();
	

#if TIME_SERIES_CHART_ENABLE_AGGREGATE_STATS
	// subplotDataSeriesIndicies_: the data series to include on a given subplot
//	std::vector<std::vector<int>> subplotDataSeriesIndicies_ = { {0, 4, 9, 13}, {1, 5, 10, 14}, {2, 6, 11, 15}, {3, 7, 12, 16}, {8, 17} };
//	std::vector<std::vector<double>> subplotDataSeriesHeights_ = { {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0}, {5.5, 10.0} };
//	std::vector<BoxPortInformation::BehavioralEventKind> variableKindVect_ = { BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
//BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
//BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
//BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
//BoxPortInformation::BehavioralEventKind::Other, BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
//BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
//BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
//BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
//BoxPortInformation::BehavioralEventKind::Other };
//	std::vector<Wt::WColor> colorVect_ = { Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255), Wt::WColor(200, 200, 200, 255), Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255), Wt::WColor(200, 200, 200, 255) };
//	std::vector<bool> isVariableAggregate_ = { false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true };
	// REMOVED RUNNING WHEEL
	//std::vector<std::vector<int>> subplotDataSeriesIndicies_ = { {0, 4, 9, 13}, {1, 5, 10, 14}, {2, 6, 11, 15}, {3, 7, 12, 16} };
	std::vector<std::vector<int>> subplotDataSeriesIndicies_ = { {0, 4, 8, 12}, {1, 5, 9, 13}, {2, 6, 10, 14}, {3, 7, 11, 15} };
	std::vector<std::vector<double>> subplotDataSeriesHeights_ = { {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0}, {2.5, 7.5, 10.0, 10.0} };
	std::vector<BoxPortInformation::BehavioralEventKind> variableKindVect_ = { BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense
};
	std::vector<Wt::WColor> colorVect_ = { Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255), Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255)};
	std::vector<bool> isVariableAggregate_ = { false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true };
#else
	std::vector<std::vector<int>> subplotDataSeriesIndicies_ = { {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8} };
	std::vector<std::vector<double>> subplotDataSeriesHeights_ = { {2.5, 7.5}, {2.5, 7.5}, {2.5, 7.5}, {2.5, 7.5}, {5.5} };
	std::vector<BoxPortInformation::BehavioralEventKind> variableKindVect_ = { BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
BoxPortInformation::BehavioralEventKind::Other };
	std::vector<Wt::WColor> colorVect_ = { Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255), Wt::WColor(200, 200, 200, 255) };
	std::vector<bool> isVariableAggregate_ = { false, false, false, false, false, false, false, false, false };

#endif // TIME_SERIES_CHART_ENABLE_AGGREGATE_STATS

	Wt::WColor otherColor_ = Wt::WColor(200, 200, 200, 255);

	static Wt::WColor make_translucent(Wt::WColor originalColor, int opacity) { return Wt::WColor(originalColor.red(), originalColor.green(), originalColor.blue(), opacity); };
	static const unsigned long long millisPerDay = 86400000;
	static const unsigned long long secondsPerDay = 86400;
	static const unsigned long long secondsPerHour = 3600;
	const double desiredAggregateBarWidth = 86400.0; // want it to span exactly 1-day.

	// gets the Wt::WDateTime corresponding to a given millisSinceEpoch in our current timezone
	static const int currentTimezoneHoursOffsetFromGMT = -4; //TODO: this changes with Daylight Savings Time (DST)
	static const Wt::WDateTime getCurrentLocalDateTimeFromMillis(unsigned long long millisSinceEpoch);

	
	static const Wt::WDateTime convertGMTTimePointToLocalDatetime(std::chrono::system_clock::time_point GMTTimePoint);
	static const Wt::WDateTime convertGMTDateTimeToLocalDatetime(Wt::WDateTime GMTDatetime);
	// the day-aggregate events should be plotted at 12noon instead of 12midnight so that they are centered on their apporpriate day. This converts datetimes from 12midnight representation to 12noon.
	static const Wt::WDateTime convertDateTimeToBarCenteredDatetime(Wt::WDateTime unshiftedLocalDatetime);

};




