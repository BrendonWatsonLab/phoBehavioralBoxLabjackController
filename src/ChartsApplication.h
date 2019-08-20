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
	static void staticUpdateActiveLabjacks();
	
private:
	ChartsExample* chartsExampleWidget = nullptr;
	void updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);
};

std::unique_ptr<WApplication> createApplication(const WEnvironment& env);

int chartsApplicationWebServer(int argc, char** argv);

