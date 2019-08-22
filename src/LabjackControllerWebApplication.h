#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <iostream>
#include "LabjackControllerOuterWidget.h"

using namespace Wt;

class LabjackControllerWebApplication : public WApplication
{
public:
	LabjackControllerWebApplication(const WEnvironment& env);
	//static void staticUpdateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
	// Called to change the number of active labjacks
	static void staticUpdateActiveLabjacks();
	// Called to refresh the data for existing labjacks
	static void staticRefreshLabjacksData();


private:
	LabjackControllerOuterWidget* labjackControllerOuterWidget = nullptr;
	void updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
};


// GLOBAL:
std::unique_ptr<WApplication> createApplication(const WEnvironment& env);

int labjackControllerApplicationWebServer(int argc, char** argv);