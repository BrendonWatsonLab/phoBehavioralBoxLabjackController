#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WEnvironment.h>
#include <Wt/WServer.h>
#include <Wt/WText.h>
#include <Wt/WTimer.h>

#include <iostream>

#include "BoxControllerWebDataServer.h"
#include "LabjackControllerOuterWidget.h"

class BehavioralBoxControllersManager;

using namespace Wt;

/* 

*/
class LabjackControllerWebApplication : public WApplication
{
public:
	LabjackControllerWebApplication(const WEnvironment& env, BoxControllerWebDataServer& server);
	//static void staticUpdateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
	// Called to change the number of active labjacks
	static void staticUpdateActiveLabjacks();
	// Called to refresh the data for existing labjacks
	static void staticRefreshLabjacksData();

	// Gets the computer's hostname
	std::string getHostName() { return this->server_.getHostName(); }
	// Gets the 2-digit integer identifier for the current computer (and box, if there is a 1-to-1 mapping). Like the "02" in "WATSON-BB-02"
	int getNumericComputerIdentifier() { return this->server_.getNumericComputerIdentifier(); }

private:
	BoxControllerWebDataServer& server_;
	Wt::WText* javaScriptError_;
	const Wt::WEnvironment& env_;
	std::unique_ptr<Wt::WTimer> timer_;
	void addOuterWidget();
	void javaScriptTest();
	void emptyFunc();

	LabjackControllerOuterWidget* labjackControllerOuterWidget = nullptr;
	void updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
};


// GLOBAL:
std::unique_ptr<WApplication> createApplication(const WEnvironment& env, BoxControllerWebDataServer& server);

int labjackControllerApplicationWebServer(int argc, char** argv, const std::shared_ptr<BehavioralBoxControllersManager>* managerPtr);