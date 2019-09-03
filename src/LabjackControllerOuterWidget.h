#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>

#include "BoxControllerWebDataServer.h"
#include "LabjackLiveStateWidget.h"

class DataServerEvent;
class TimeSeriesChart;
class WebAppHelpWidget;

/*! \brief A widget that demonstrates various aspects of the charting lib.
 */
class LabjackControllerOuterWidget : public Wt::WContainerWidget, public BoxControllerWebDataServer::Client
{
public:
	/*! \brief Constructor.
	 */
	LabjackControllerOuterWidget(BoxControllerWebDataServer& server);
	~LabjackControllerOuterWidget();

	// Server stuff
	BoxControllerWebDataServer& server() { return server_; }
	void connect();
	void disconnect();

	void setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks);

	//requestServerHistoricalData(): requests the most recent version of the historical data (without reloading)
	void requestServerHistoricalData();
	//requestServerHistoricalDataReload(): requests a reload of historical data.
	void requestServerHistoricalDataReload();

protected:
	bool loggedIn() const;

#if ENABLE_WEB_SERVER_LIVE_WIDGET
	LabjackLiveStateWidget* labjackExampleWidget;
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET
	TimeSeriesChart* timeSeriesChartWidget;
	WebAppHelpWidget* webAppHelpWidget;

private:
	std::string appName = "";
	Wt::WVBoxLayout* mainLayout_;

	// Server Stuff:
	BoxControllerWebDataServer& server_;
	bool loggedIn_;
	/* called from another session */
	void processDataServerEvent(const DataServerEvent& event);
	std::vector<std::string> loadedHistoricalDataVectIDs_;
	int currActiveLabjackIndex_ = 0;

	// Header:
	void setupHeader();
	Wt::WContainerWidget* headerRootContainer_;
	Wt::WNavigationBar* navigation_;
	Wt::WStackedWidget* contentsStack_;
	Wt::WText* activeLabjackName_;

	void updateActiveLabjackInfo();

	//void home();
	//void sidebar();
	//void footer();
};

/*@}*/