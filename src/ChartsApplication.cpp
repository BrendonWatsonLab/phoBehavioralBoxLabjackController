/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication.h>
//#include "ChartsExample.h"
#include "ChartsApplication.h"
#include "LabjackHelpers.h"

using namespace Wt;

ChartsApplication::ChartsApplication(const WEnvironment& env): WApplication(env)
{
	this->enableUpdates(true);
	WApplication::instance()->enableUpdates(true);
    setTitle("Charts example");

    setCssTheme("polished");
    messageResourceBundle().use(appRoot() + "resources/" + "charts");

    root()->setPadding(10);
    root()->resize(WLength::Auto, WLength::Auto);

	this->chartsExampleWidget = root()->addWidget(cpp14::make_unique<ChartsExample>());

    /*
     * Set our style sheet last, so that it loaded after the ext stylesheets.
     */
    useStyleSheet("resources/charts.css");
 }

void ChartsApplication::staticUpdateActiveLabjacks()
{
	ChartsApplication* app = dynamic_cast<ChartsApplication*>(WApplication::instance());
	if (app) {
		std::vector<BehavioralBoxLabjack*> activeLabjacks = LabjackHelpers::findAllLabjacks();
		app->updateActiveLabjacks(activeLabjacks);
	}
	else {
		// this cannot really happen
	}
}

  //void ChartsApplication::staticUpdateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks)
  //{
	 // ChartsApplication* app = dynamic_cast<ChartsApplication*>(WApplication::instance());
	 // if (app) {
		//  app->updateActiveLabjacks(updatedLabjacks);
	 // }
	 // else {
		//  // this cannot really happen
	 // }
  //}

  void ChartsApplication::updateActiveLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks)
  {
	  if (this->chartsExampleWidget != NULL) {
		  this->chartsExampleWidget->setActiveLabjacks(updatedLabjacks);
		  WApplication::instance()->triggerUpdate();
		  this->triggerUpdate();
		  return;
	  }
	  else {
		  printf("Error getting chartsExampleWidget!!\n");
		  return;
	  }

  }

std::unique_ptr<WApplication> createApplication(const WEnvironment& env)
{
  return cpp14::make_unique<ChartsApplication>(env);
}

int chartsApplicationWebServer(int argc, char** argv)
{
  return WRun(argc, argv, &createApplication);
}



//int main(int argc, char **argv)
//{
//  return WRun(argc, argv, &createApplication);
//}
