#pragma once
#include <Wt/WContainerWidget.h>
#include <iostream>
#include "LabjackLiveStateWidget.h"

/*! \brief A widget that demonstrates various aspects of the charting lib.
 */
class LabjackControllerOuterWidget : public Wt::WContainerWidget
{
public:
	/*! \brief Constructor.
	 */
	LabjackControllerOuterWidget();

	void setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks);
#if ENABLE_WEB_SERVER_LIVE_WIDGET
	LabjackLiveStateWidget* labjackExampleWidget;
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET

private:
	std::string appName = "";

	//void header();
	//void home();
	//void sidebar();
	//void footer();
};

/*@}*/