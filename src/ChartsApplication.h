#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <iostream>
#include "ChartsExample.h"

using namespace Wt;

class ChartsApplication : public WApplication
{
public:
	ChartsApplication(const WEnvironment& env);
	//static void staticUpdateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
	// Called to change the number of active labjacks
	static void staticUpdateActiveLabjacks();
	// Called to refresh the data for existing labjacks
	static void staticRefreshLabjacksData();

	
private:
	ChartsExample* chartsExampleWidget = nullptr;
	void updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
};

//std::unique_ptr<WApplication> createApplication(const WEnvironment& env);
//
//int chartsApplicationWebServer(int argc, char** argv);

