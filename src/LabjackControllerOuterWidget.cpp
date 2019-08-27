#include <Wt/WApplication.h>
#include "LabjackControllerOuterWidget.h"
#include "TimeSeriesChart.h"

using namespace Wt;

LabjackControllerOuterWidget::LabjackControllerOuterWidget() : WContainerWidget()
{
	this->appName = "Labjack Manager Overview:";
	this->addWidget(cpp14::make_unique<WText>(this->appName));

	this->labjackExampleWidget = this->addWidget(cpp14::make_unique<LabjackLiveStateWidget>());
	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);
	this->addWidget(cpp14::make_unique<TimeSeriesChart>());

}

void LabjackControllerOuterWidget::setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks)
{
	this->labjackExampleWidget->updateLabjacks(activeLabjacks);
}
