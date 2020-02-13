#include <Wt/WApplication.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include "LabjackControllerWebApplication.h"
#include "LabjackHelpers.h"

using namespace Wt;

LabjackControllerWebApplication::LabjackControllerWebApplication(const WEnvironment& env, BoxControllerWebDataServer& server) : WApplication(env), server_(server), env_(env)
{
	this->enableUpdates(true);
	WApplication::instance()->enableUpdates(true);

	setTitle("Labjack Controller Web App");
	// TODO: Change to include BBID

	setCssTheme("polished");
	messageResourceBundle().use(appRoot() + "resources/" + "charts");

	root()->setPadding(10);
	root()->resize(WLength::Auto, WLength::Auto);

	//// Loading Indicator
	//auto loadingIndicatorWidget = root()->addWidget(cpp14::make_unique<WOverlayLoadingIndicator>());
	//loadingIndicatorWidget->setMessage("Loading History, please wait...");

	// Main Widget
	this->addOuterWidget();


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

void LabjackControllerWebApplication::addOuterWidget()
{
	this->labjackControllerOuterWidget = root()->addWidget(cpp14::make_unique<LabjackControllerOuterWidget>(server_));
	//this->labjackControllerOuterWidget->setStyleClass("chat");
}

void LabjackControllerWebApplication::javaScriptTest()
{
	if (!env_.javaScript()) {
		javaScriptError_ =
			root()->addWidget(Wt::cpp14::make_unique<Wt::WText>(Wt::WString::tr("serverpushwarning")));

		// The 5 second timer is a fallback for real server push. The updated
		// server state will piggy back on the response to this timeout.
		timer_ = Wt::cpp14::make_unique<Wt::WTimer>();
		timer_->setInterval(std::chrono::milliseconds{ 5000 });
		timer_->timeout().connect(this, &LabjackControllerWebApplication::emptyFunc);
		timer_->start();
	}
}

void LabjackControllerWebApplication::emptyFunc()
{
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
std::unique_ptr<WApplication> createApplication(const WEnvironment& env, BoxControllerWebDataServer& server)
{
	return cpp14::make_unique<LabjackControllerWebApplication>(env, server);
}

int labjackControllerApplicationWebServer(int argc, char** argv, const std::shared_ptr<BehavioralBoxControllersManager>* managerPtr)
{
	Wt::WServer server(argc, argv, WTHTTP_CONFIGURATION);
	BoxControllerWebDataServer dataServer(server, managerPtr);

	/*
   * We add two entry points: one for the full-window application,
   * and one for a widget that can be integrated in another page.
   */
	server.addEntryPoint(Wt::EntryPointType::Application, std::bind(createApplication, std::placeholders::_1, std::ref(dataServer)));
	//server.addEntryPoint(Wt::EntryPointType::WidgetSet, std::bind(createWidget, std::placeholders::_1, std::ref(dataServer)), "/chat.js");

	if (server.start()) {
		int sig = Wt::WServer::waitForShutdown();
		std::cerr << "Shutting down: (signal = " << sig << ")" << std::endl;
		server.stop();
		return sig;
	}
	else {
		return -1;
	}
	
}