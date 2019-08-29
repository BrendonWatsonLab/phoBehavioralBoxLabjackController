#include <Wt/WApplication.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include "LabjackControllerWebApplication.h"
#include "LabjackHelpers.h"

using namespace Wt;

LabjackControllerWebApplication::LabjackControllerWebApplication(const WEnvironment& env) : WApplication(env)
{
	this->enableUpdates(true);
	WApplication::instance()->enableUpdates(true);

	setTitle("Labjack Controller Web App");

	setCssTheme("polished");
	messageResourceBundle().use(appRoot() + "resources/" + "charts");

	root()->setPadding(10);
	root()->resize(WLength::Auto, WLength::Auto);

	//// Loading Indicator
	//auto loadingIndicatorWidget = root()->addWidget(cpp14::make_unique<WOverlayLoadingIndicator>());
	//loadingIndicatorWidget->setMessage("Loading History, please wait...");

	// Main Widget
	this->labjackControllerOuterWidget = root()->addWidget(cpp14::make_unique<LabjackControllerOuterWidget>());


	/*
	 * Set our style sheet last, so that it loaded after the ext stylesheets.
	 */
	useStyleSheet("resources/charts.css");
}

void LabjackControllerWebApplication::staticUpdateActiveLabjacks()
{
	LabjackControllerWebApplication* app = dynamic_cast<LabjackControllerWebApplication*>(WApplication::instance());
	assert(app != nullptr);
	// Find the labjacks again.
	std::vector<BehavioralBoxLabjack*> activeLabjacks = LabjackHelpers::findAllLabjacks();
	app->updateActiveLabjacks(activeLabjacks);
}

void LabjackControllerWebApplication::staticRefreshLabjacksData()
{
	//TODO
}

void LabjackControllerWebApplication::updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks)
{
	// Loading Indicator
	//auto loadingIndicatorWidget = root()->addWidget(cpp14::make_unique<WOverlayLoadingIndicator>());
	//loadingIndicatorWidget->setMessage("Loading History, please wait...");
	this->setLoadingIndicator(cpp14::make_unique<WOverlayLoadingIndicator>());


	if (this->labjackControllerOuterWidget != NULL) {
		this->labjackControllerOuterWidget->setActiveLabjacks(updatedLabjacks);
		WApplication::instance()->triggerUpdate();
		this->triggerUpdate();
		return;
	}
	else {
		printf("Error getting labjackControllerOuterWidget!!\n");
		return;
	}
}


//GLOBAL:
std::unique_ptr<WApplication> createApplication(const WEnvironment& env)
{
	return cpp14::make_unique<LabjackControllerWebApplication>(env);
}

int labjackControllerApplicationWebServer(int argc, char** argv)
{
	return WRun(argc, argv, &createApplication);
}