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
	LabjackLiveStateWidget* labjackExampleWidget;

private:
	std::string appName = "";

	//void header();
	//void home();
	//void sidebar();
	//void footer();
};

/*@}*/