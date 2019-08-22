#include <Wt/WApplication.h>
#include "LabjackControllerOuterWidget.h"

using namespace Wt;

LabjackControllerOuterWidget::LabjackControllerOuterWidget() : WContainerWidget()
{
	this->appName = "Labjack Manager Overview:";
	this->addWidget(cpp14::make_unique<WText>(this->appName));

	this->labjackExampleWidget = this->addWidget(cpp14::make_unique<LabjackExample>());

}

void LabjackControllerOuterWidget::setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks)
{
	this->labjackExampleWidget->updateLabjacks(activeLabjacks);
}
