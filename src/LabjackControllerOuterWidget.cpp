#include <Wt/WApplication.h>
#include "LabjackControllerOuterWidget.h"
#include "TimeSeriesChart.h"
#include "WebAppHelpWidget.h"

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

	// Timeseries Charts:
	this->addWidget(cpp14::make_unique<TimeSeriesChart>());

	// Help Panel:
	this->addWidget(cpp14::make_unique<WebAppHelpWidget>());

}

void LabjackControllerOuterWidget::setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks)
{
#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget->updateLabjacks(activeLabjacks);
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET	
}
