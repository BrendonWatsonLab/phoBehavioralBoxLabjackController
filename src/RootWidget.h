#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>

#include "BoxControllerWebDataServer.h"

class DataServerEvent;
struct BehavioralBoxDataWidgetConfiguration;
class BehavioralBoxDataWidget;

class RootWidget : public Wt::WContainerWidget, public BoxControllerWebDataServer::Client
{
public:
	/*! \brief Constructor.
	 */
	RootWidget(BoxControllerWebDataServer& server);
	~RootWidget();

	// Server stuff
	BoxControllerWebDataServer& server() { return server_; }
	void connect();
	void disconnect();

	//requestServerHistoricalData(): requests the most recent version of the historical data (without reloading)
	void requestServerHistoricalData();
	//requestServerHistoricalDataReload(): requests a reload of historical data.
	void requestServerHistoricalDataReload();

	void updateBehavioralBoxWidgets();

protected:
	bool loggedIn() const;
	std::vector<BehavioralBoxDataWidget*> behavioralBoxWidgets;

private:
	std::string appName = "";
	std::string hostName = "";
	Wt::WVBoxLayout* mainLayout_;

	// Server Stuff:
	BoxControllerWebDataServer& server_;
	bool loggedIn_;
	/* called from another session */
	void processDataServerEvent(const DataServerEvent& event);

	bool is_first_time_settingUp_boxes = true;
	std::vector<std::string> loadedHistoricalDataVectIDs_;
	//std::vector<BehavioralBoxHistoricalData> loadedHistoricalDataVect_;
	std::vector<BehavioralBoxDataWidgetConfiguration> loadedBehavioralBoxDataWidgetConfigs_;

	// Header:
	void setupHeader();
	Wt::WContainerWidget* headerRootContainer_;
	Wt::WText* lblNumberOfBehavioralBoxesFound_;
	Wt::WNavigationBar* navigation_;
	Wt::WStackedWidget* contentsStack_;


};

