#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>

class TimeSeriesChart;
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

private:
	BehavioralBoxDataWidgetConfiguration configuration;
	Wt::WVBoxLayout* mainLayout_;

	// Header:
	void setupHeader();
	Wt::WContainerWidget* headerRootContainer_;
	Wt::WStackedWidget* contentsStack_;

	Wt::WText* lblActiveBBIDName_;
	Wt::WText* lblActiveFilePath_;
	Wt::WText* lblActiveDataRangeDescription_;

	//void home();
	//void sidebar();
	//void footer();
};

