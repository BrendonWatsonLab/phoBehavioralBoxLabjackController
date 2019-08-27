#include <Wt/WApplication.h>
#include "LabjackControllerOuterWidget.h"
#include "TimeSeriesChart.h"

using namespace Wt;

LabjackControllerOuterWidget::LabjackControllerOuterWidget() : WContainerWidget()
{
	this->appName = "Labjack Manager Overview:";
	this->addWidget(cpp14::make_unique<WText>(this->appName));

#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget = this->addWidget(cpp14::make_unique<LabjackLiveStateWidget>());
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET

	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);
	this->addWidget(cpp14::make_unique<TimeSeriesChart>());

}

void LabjackControllerOuterWidget::setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks)
{
#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget->updateLabjacks(activeLabjacks);
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET	
}
