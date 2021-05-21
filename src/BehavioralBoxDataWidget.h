#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>

#define PHO_INCLUDE_TIME_SERIES_CHART

#ifdef PHO_INCLUDE_TIME_SERIES_CHART
	class TimeSeriesChart;
#endif // PHO_INCLUDE_TIME_SERIES_CHART


class WebAppHelpWidget;

#include "BehavioralBoxHistoricalData.h"

// Configuration for the widget:
struct BehavioralBoxDataWidgetConfiguration
{
	/*int bbID = -1;*/
	std::string bbIDString = "";
	BehavioralBoxHistoricalData data;

	int numberOfFiles = 0;
	std::string fileSearchPath = "";
	
	std::string oldestFileDate = "";
	std::string newestFileDate = "";

	BehavioralBoxDataWidgetConfiguration() {
		//this->newestFileDate = std::string(data.getFinalMillisecondsSinceEpoch());
	}

	BehavioralBoxDataWidgetConfiguration(BehavioralBoxHistoricalData data): data(data), bbIDString(data.getBoxIdentifier()), fileSearchPath(data.getSearchDirectory()) {
		//this->newestFileDate = std::string(data.getFinalMillisecondsSinceEpoch());
		this->numberOfFiles = data.getNumberOfDataFiles();

		// Get earliest and latest:
		auto dataFileTimestamps = data.getDataFileTimestamps();
		if (dataFileTimestamps.size() == 0)
		{
			this->oldestFileDate = "";
			this->newestFileDate = "";
		}
		else if (dataFileTimestamps.size() == 1) {
			this->oldestFileDate = "";
			this->newestFileDate = FormattingHelper::format_datetime(dataFileTimestamps[0]);
		}
		else {
			// (dataFileTimestamps.size() >= 2)
			this->oldestFileDate = FormattingHelper::format_datetime(dataFileTimestamps[0]);
			this->newestFileDate = FormattingHelper::format_datetime(dataFileTimestamps[dataFileTimestamps.size()-1]);
		}

	}

	std::string getDataDescriptionString() {
		return std::to_string(numberOfFiles) + " files from " + oldestFileDate + " to " + newestFileDate;
	}

};

// DataWidget for a single Behavioral Box:
class BehavioralBoxDataWidget : public Wt::WContainerWidget
{
public:
	/*! \brief Constructor.
	 */
	BehavioralBoxDataWidget();
	BehavioralBoxDataWidget(BehavioralBoxDataWidgetConfiguration configuration);
	~BehavioralBoxDataWidget();


	//requestServerHistoricalData(): requests the most recent version of the historical data (without reloading)
	void requestHistoricalData();
	//requestServerHistoricalDataReload(): requests a reload of historical data.
	//void requestHistoricalDataReload();


	BehavioralBoxDataWidgetConfiguration getConfiguration() {
		return this->configuration;
	}

	void updateConfiguration(BehavioralBoxDataWidgetConfiguration new_config) {
		this->configuration = new_config;
		// Refresh the widget or something;
	}

	void refresh();

protected:
	
	//TimeSeriesChart* timeSeriesChartWidget;
	//WebAppHelpWidget* webAppHelpWidget;

	#ifdef PHO_INCLUDE_TIME_SERIES_CHART
		TimeSeriesChart* timeSeriesChartWidget;
	#endif // PHO_INCLUDE_TIME_SERIES_CHART

private:
	BehavioralBoxDataWidgetConfiguration configuration;
	Wt::WVBoxLayout* mainLayout_;

	// Header:
	void setupHeader();
	Wt::WContainerWidget* headerRootContainer_;
	/*Wt::WStackedWidget* contentsStack_;*/
	Wt::WContainerWidget* contentsContainer_;

	Wt::WText* lblActiveBBIDName_;
	Wt::WText* lblActiveFilePath_;
	Wt::WText* lblActiveDataRangeDescription_;

	//void home();
	//void sidebar();
	//void footer();
};

