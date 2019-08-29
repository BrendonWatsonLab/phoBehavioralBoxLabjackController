#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>

#include "BoxControllerWebDataServer.h"
#include "LabjackLiveStateWidget.h"


class DataServerEvent;

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

#if ENABLE_WEB_SERVER_LIVE_WIDGET
	LabjackLiveStateWidget* labjackExampleWidget;
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET

protected:
	bool loggedIn() const;

private:
	std::string appName = "";

	// Server Stuff:
	BoxControllerWebDataServer& server_;
	bool loggedIn_;
	/* called from another session */
	void processDataServerEvent(const DataServerEvent& event);

	//void header();
	//void home();
	//void sidebar();
	//void footer();
};

/*@}*/