#pragma once
#include <vector>
#include <string>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WAbstractItemModel.h>

#include "BoxPortInformation.h"

class BehavioralBoxHistoricalData;
class HistoricalDataLoadingEvent;

class TimeSeriesChart : public Wt::WContainerWidget
{
public:
	TimeSeriesChart();

	bool shouldUseDateXAxis = true;

	void reload(std::vector<BehavioralBoxHistoricalData> historicalData);

	std::vector<Wt::WColor> getVariableColors() { return this->colorVect_; }
	Wt::WColor getDefaultColor() { return this->otherColor_; }

	std::shared_ptr<Wt::WAbstractItemModel> model;

	// Update Function:
	void processHistoricalDataUpdateEvent(const HistoricalDataLoadingEvent& event);

private:
	// Config:
	bool shouldEnableAggregateStatistics_ = true;

	// UI Elements
	WContainerWidget* loadingContainerWidget;
	WContainerWidget* tableContainerWidget;
	WContainerWidget* chartsContainerWidget;

	void setupLoadingIndicator();
	void setupTable(const std::shared_ptr<Wt::WAbstractItemModel> model);
	void setupCharts(const std::shared_ptr<Wt::WAbstractItemModel> model);

	std::shared_ptr<Wt::WStandardItemModel> buildHistoricDataModel(std::vector<BehavioralBoxHistoricalData> historicalData);
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> buildDataSeries(const std::shared_ptr<Wt::WAbstractItemModel> model);

	// Data Series:
	const int getNumSubplots() { return this->subplotDataSeriesIndicies_.size(); }; // 4 food/water signals + 1 for running wheel
	const int getNumVariables() { return this->colorVect_.size(); };
	// subplotDataSeriesIndicies_: the data series to include on a given subplot
	std::vector<std::vector<int>> subplotDataSeriesIndicies_ = { {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8} };
	std::vector<std::vector<double>> subplotDataSeriesHeights_ = { {2.5, 7.5}, {2.5, 7.5}, {2.5, 7.5}, {2.5, 7.5}, {5.5} };
	std::vector<BoxPortInformation> variableVect_;

	std::vector<BoxPortInformation::BehavioralEventKind> variableKindVect_ = { BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
	BoxPortInformation::BehavioralEventKind::BeamBreak, BoxPortInformation::BehavioralEventKind::BeamBreak,
	BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
	BoxPortInformation::BehavioralEventKind::Dispense, BoxPortInformation::BehavioralEventKind::Dispense,
	BoxPortInformation::BehavioralEventKind::Other};
	
	std::vector<Wt::WColor> colorVect_ = { Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255), Wt::WColor(200, 200, 200, 255) };
	Wt::WColor otherColor_ = Wt::WColor(200, 200, 200, 255);

	static Wt::WColor make_translucent(Wt::WColor originalColor, int opacity) { return Wt::WColor(originalColor.red(), originalColor.green(), originalColor.blue(), opacity); };
};




