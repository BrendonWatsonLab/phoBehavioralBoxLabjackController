#include <Wt/WApplication.h>
#include <Wt/WAnimation.h>
#include <Wt/WPanel.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WMenu.h>
#include <Wt/WNavigationBar.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

#include "BehavioralBoxDataWidget.h"
#include "TimeSeriesChart.h"
#include "WebAppHelpWidget.h"

using namespace Wt;

BehavioralBoxDataWidget::BehavioralBoxDataWidget() : BehavioralBoxDataWidget(BehavioralBoxDataWidgetConfiguration())
{
}

BehavioralBoxDataWidget::BehavioralBoxDataWidget(BehavioralBoxDataWidgetConfiguration configuration) : WContainerWidget(), configuration(configuration)
{
	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);

	// Setup main layout
	mainLayout_ = this->setLayout(Wt::cpp14::make_unique<Wt::WVBoxLayout>());

	auto contentsStack = Wt::cpp14::make_unique<Wt::WStackedWidget>();
	contentsStack_ = contentsStack.get();

	//// Timeseries Charts:
	//this->timeSeriesChartWidget = this->contentsStack_->addWidget(cpp14::make_unique<TimeSeriesChart>());

	//// Help Panel:
	//this->webAppHelpWidget = this->contentsStack_->addWidget(cpp14::make_unique<WebAppHelpWidget>());

	// Build Header:
	this->setupHeader();

	/*
	 * Add it all inside a layout
	 */
	this->mainLayout_->addWidget(std::move(contentsStack), 2);
	this->mainLayout_->setContentsMargins(0, 0, 0, 0);

	// Request an update
	this->requestHistoricalData();
}

BehavioralBoxDataWidget::~BehavioralBoxDataWidget()
{
}

void BehavioralBoxDataWidget::requestHistoricalData()
{
}

void BehavioralBoxDataWidget::refresh()
{
	// Update the time series chart widget
	//this->timeSeriesChartWidget->processHistoricalDataUpdateEvent(historicalEvent);
}

void BehavioralBoxDataWidget::setupHeader()
{
	setOverflow(Wt::Overflow::Hidden);

	auto headerRootContainer = Wt::cpp14::make_unique<Wt::WContainerWidget>();
	headerRootContainer_ = headerRootContainer.get();

	auto inactiveActiveLabjackLabel = this->headerRootContainer_->addWidget(std::make_unique<WText>("BBID: "));
	this->activeBBIDName_ = this->headerRootContainer_->addWidget(std::make_unique<WText>("None"));


	// Add it to the layout
	this->mainLayout_->addWidget(std::move(headerRootContainer), 0);
}
