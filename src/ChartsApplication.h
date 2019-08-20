#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <iostream>

using namespace Wt;

std::unique_ptr<WApplication> createApplication(const WEnvironment& env);
int chartsApplicationWebServer(int argc, char** argv);
