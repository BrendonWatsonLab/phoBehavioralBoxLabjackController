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
	this->lblActiveBBIDName_->setText(this->configuration.bbIDString);
	// Make the string
	std::string dataDescriptionString = std::to_string(this->configuration.numberOfFiles) + " files from " + this->configuration.oldestFileDate + " to " + this->configuration.newestFileDate;
	this->lblActiveDataRangeDescription_->setText(this->configuration.bbIDString);
	this->lblActiveFilePath_->setText(this->configuration.fileSearchPath);


}

void BehavioralBoxDataWidget::setupHeader()
{
	setOverflow(Wt::Overflow::Hidden);

	auto headerRootContainer = Wt::cpp14::make_unique<Wt::WContainerWidget>();
	headerRootContainer_ = headerRootContainer.get();

	// Setup main layout
	auto headerRootContainer_mainLayout = headerRootContainer_->setLayout(std::make_unique<Wt::WVBoxLayout>());

	auto headerRootContainer_TopRow = Wt::cpp14::make_unique<Wt::WContainerWidget>();
	auto headerRootContainer_TopRow_Layout = headerRootContainer_TopRow->setLayout(std::make_unique<Wt::WHBoxLayout>());
	headerRootContainer_TopRow_Layout->setContentsMargins(2,2,2,2);

	auto headerRootContainer_BottomRow = Wt::cpp14::make_unique<Wt::WContainerWidget>();
	auto headerRootContainer_BottomRow_Layout = headerRootContainer_BottomRow->setLayout(std::make_unique<Wt::WHBoxLayout>());
	headerRootContainer_BottomRow_Layout->setContentsMargins(2, 2, 2, 2);

	auto inactiveActiveLabjackLabel = headerRootContainer_TopRow_Layout->addWidget(std::make_unique<WText>("BBID: "));
	this->lblActiveBBIDName_ = headerRootContainer_TopRow_Layout->addWidget(std::make_unique<WText>("Loading..."));
	this->lblActiveDataRangeDescription_ = headerRootContainer_TopRow_Layout->addWidget(std::make_unique<WText>(""));

	this->lblActiveFilePath_ = headerRootContainer_BottomRow_Layout->addWidget(std::make_unique<WText>(""));



	//auto inactiveActiveLabjackLabel = this->headerRootContainer_->addWidget(std::make_unique<WText>("BBID: "));
	//this->lblActiveBBIDName_ = this->headerRootContainer_->addWidget(std::make_unique<WText>("Loading..."));
	//this->lblActiveDataRangeDescription_ = this->headerRootContainer_->addWidget(std::make_unique<WText>(""));
	//this->lblActiveFilePath_ = this->headerRootContainer_->addWidget(std::make_unique<WText>(""));



	/*
	 * Add it all inside a layout
	 */
	headerRootContainer_mainLayout->addWidget(std::move(headerRootContainer_TopRow), 2);
	headerRootContainer_mainLayout->addWidget(std::move(headerRootContainer_BottomRow), 2);
	headerRootContainer_mainLayout->setContentsMargins(0, 0, 0, 0);



	// Add it to the layout
	this->mainLayout_->addWidget(std::move(headerRootContainer), 0);
}
