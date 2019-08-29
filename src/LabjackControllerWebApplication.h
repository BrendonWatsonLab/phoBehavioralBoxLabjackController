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

using namespace Wt;

class LabjackControllerWebApplication : public WApplication
{
public:
	LabjackControllerWebApplication(const WEnvironment& env, BoxControllerWebDataServer& server);
	//static void staticUpdateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
	// Called to change the number of active labjacks
	static void staticUpdateActiveLabjacks();
	// Called to refresh the data for existing labjacks
	static void staticRefreshLabjacksData();


private:
	BoxControllerWebDataServer& server_;
	Wt::WText* javaScriptError_;
	const Wt::WEnvironment& env_;
	std::unique_ptr<Wt::WTimer>   timer_;
	/*! \brief Add another chat client.
   */
	void addOuterWidget();
	void javaScriptTest();
	void emptyFunc();

	LabjackControllerOuterWidget* labjackControllerOuterWidget = nullptr;
	void updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
};


// GLOBAL:
std::unique_ptr<WApplication> createApplication(const WEnvironment& env, BoxControllerWebDataServer& server);

int labjackControllerApplicationWebServer(int argc, char** argv);