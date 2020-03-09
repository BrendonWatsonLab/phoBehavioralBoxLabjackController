#include "RootWidget.h"
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

#include "DataServerEvent.h"
#include "BehavioralBoxDataWidget.h"

using namespace Wt;


RootWidget::RootWidget(BoxControllerWebDataServer& server) : WContainerWidget(), server_(server), loggedIn_(false)
{
	// TODO: Set appName to BBID
// Gets the computer's hostname
	this->hostName = this->server_.getHostName();
	this->appName = "Behavioral Box Overseer: Data Overview: " + this->hostName;

	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);

	// Setup main layout
	mainLayout_ = this->setLayout(std::make_unique<Wt::WVBoxLayout>());

	//auto contentsStack = Wt::cpp14::make_unique<Wt::WStackedWidget>();
	//contentsStack_ = contentsStack.get();

	auto contentsStack = std::make_unique<Wt::WContainerWidget>();
	contentsStack_ = contentsStack.get();

	// Build Header:
	this->setupHeader();

	/*
	 * Add it all inside a layout
	 */
	this->mainLayout_->addWidget(std::move(contentsStack), 2);
	this->mainLayout_->setContentsMargins(0, 0, 0, 0);

	this->connect();
	loggedIn_ = true;

	// Request an update
	//this->requestServerHistoricalDataReload();
	this->requestServerHistoricalData();

}

RootWidget::~RootWidget()
{
	if (loggedIn()) {
		loggedIn_ = false;
		disconnect();
	}
}

void RootWidget::connect()
{
	if (server_.connect(this, std::bind(&RootWidget::processDataServerEvent, this, std::placeholders::_1))) {
		Wt::WApplication::instance()->enableUpdates(true);
	}
}

void RootWidget::disconnect()
{
	if (server_.disconnect(this)) {
		Wt::WApplication::instance()->enableUpdates(false);
	}
}

void RootWidget::requestServerHistoricalData()
{
	this->server_.requestHistoricalData();
}

void RootWidget::requestServerHistoricalDataReload()
{
	this->server_.requestHistoricalDataReload();
}

void RootWidget::updateBehavioralBoxWidgets()
{
	Wt::WApplication* app = Wt::WApplication::instance();
	if (this->behavioralBoxWidgets.size() > 0) {
		this->lblNumberOfBehavioralBoxesFound_->setText(std::to_string(this->loadedBehavioralBoxDataWidgetConfigs_.size()));
	}
	else {
		this->lblNumberOfBehavioralBoxesFound_->setText("None");
	}

	for (size_t i = 0; i < this->behavioralBoxWidgets.size(); i++)
	{
		this->behavioralBoxWidgets[i]->updateConfiguration(this->loadedBehavioralBoxDataWidgetConfigs_[i]);
		this->behavioralBoxWidgets[i]->refresh();
	}
	app->triggerUpdate();
}

bool RootWidget::loggedIn() const
{
	return loggedIn_;
}

void RootWidget::processDataServerEvent(const DataServerEvent& event)
{
	Wt::WApplication* app = Wt::WApplication::instance();

	/*
	 * This is where the "server-push" happens. The data server posts to this
	 * event from other sessions, see SimpleChatServer::postChatEvent()
	 */
	if (event.type() == DataServerEvent::Type::HistoricalDataRefreshed) {
		std::cout << "RootWidget::processDataServerEvent(...): Historical data refreshed." << std::endl;
		auto historicalEvent = event.historicalDataLoadingEvent();
		if (historicalEvent.type() == HistoricalDataLoadingEvent::Type::Complete) {
			// 

			auto prev_vectIDs = this->loadedHistoricalDataVectIDs_;
			int numPrevVectIDs = prev_vectIDs.size();
			int prevVectIDsMaxIndex = numPrevVectIDs - 1;

			std::vector<BehavioralBoxHistoricalData> loadedHistoricalDataVect = historicalEvent.dataLoadedHistoricalDataVector();
			int numLoadedVectIDs = loadedHistoricalDataVect.size();

			if (this->is_first_time_settingUp_boxes) {
				std::cout << "Setting up boxes for the first time... " << std::endl;

			}
			else {
				// Just updating existing boxes:
				std::cout << "Updating existing boxes... " << std::endl;
				
			}

			this->loadedHistoricalDataVectIDs_.clear(); // Clear old vect IDs
			this->loadedBehavioralBoxDataWidgetConfigs_.clear(); // Clear old configuration

			bool loadedBBsChanged = false; // true if the number or info of the BBs changed since last time
			bool needCreateNewWidget = true; // true if we need to create a new outer widget for this box

			for (int i = 0; i < numLoadedVectIDs; i++)
			{
				BehavioralBoxDataWidgetConfiguration curr_widget_config = BehavioralBoxDataWidgetConfiguration(loadedHistoricalDataVect[i]);
				std::string currBoxIdentifier = curr_widget_config.bbIDString;

				if (i <= prevVectIDsMaxIndex)
				{
					needCreateNewWidget = false; // don't need to create a new widget because we'll reuse this one
					// Get the old BBID at this index
					std::string prevBoxIdentifier = prev_vectIDs[i];
					if (prevBoxIdentifier != currBoxIdentifier) {
						// They aren't the same, so it must have changed!
						loadedBBsChanged = true;
					}
					// Otherwise it's a match, it just needs to be updated.
				}
				else {
					// The current index exceeds the size of the old vect IDs, BBs must have changed
					loadedBBsChanged = true;
					needCreateNewWidget = true;

				}
				// Add the current BBID to the vector
				this->loadedHistoricalDataVectIDs_.push_back(currBoxIdentifier);
				this->loadedBehavioralBoxDataWidgetConfigs_.push_back(curr_widget_config);

				if (needCreateNewWidget) {
					// Create the new widget for this BBID:
					std::cout << "Creating new BB Widget for BBID: " << currBoxIdentifier << std::endl;

					// Note: even replacing the widget with a simple WText doesn't add multiple of them to the widget hierarchy.
					/*this->contentsStack_->addWidget<WText>(std::make_unique<WText>("Test"));*/
					this->behavioralBoxWidgets.push_back(this->contentsStack_->addNew<BehavioralBoxDataWidget>(curr_widget_config));

					/*this->contentsStack_->addNew<Wt::WText>(Wt::WString("<p>Text {1}</p>").arg(currBoxIdentifier));*/
					needCreateNewWidget = false;
				}

				// Handle what to do if the BB info itself changed:
				//if (loadedBBsChanged) {
				//	//TODO: Replace the info in the widget at this index with the new info.
				//	std::cout << "Reloading BB Widget for BBID: " << currBoxIdentifier << std::endl;
				//	this->updateBehavioralBoxWidgets();
				//}

			} // end for loop

			//this->updateBehavioralBoxWidgets();
			this->is_first_time_settingUp_boxes = false;
		}

		//this->updateBehavioralBoxWidgets();
		//// Update the time series chart widget
		//this->timeSeriesChartWidget->processHistoricalDataUpdateEvent(historicalEvent);
	}
	else {
		std::cout << "Warning: DataServerEvent Event recieved but not yet implemented!" << std::endl;
	}

	///*
	// * This is the server call: we (schedule to) propagate the updated UI to
	// * the client.
	// *
	// * This schedules an update and returns immediately
	// */
	this->updateBehavioralBoxWidgets();
	app->triggerUpdate();
}


void RootWidget::setupHeader()
{
	setOverflow(Wt::Overflow::Hidden);

	auto headerRootContainer = Wt::cpp14::make_unique<Wt::WContainerWidget>();
	headerRootContainer_ = headerRootContainer.get();

	auto navigation = Wt::cpp14::make_unique<Wt::WNavigationBar>();
	navigation_ = navigation.get();

	std::string access_url = "http://" + this->hostName + ":8080";
	navigation_->addStyleClass("main-nav");
	//navigation_->setTitle(this->appName, "http://127.0.0.1:8080");
	navigation_->setTitle(this->appName, access_url);
	//navigation_->setResponsive(true);

	auto inactiveActiveLabjackLabel = this->headerRootContainer_->addWidget(cpp14::make_unique<WText>("Behavioral Boxes: "));
	this->lblNumberOfBehavioralBoxesFound_ = this->headerRootContainer_->addWidget(cpp14::make_unique<WText>("Loading..."));


	// Add it to the layout
	this->mainLayout_->addWidget(std::move(navigation), 0);
	this->mainLayout_->addWidget(std::move(headerRootContainer), 1);

}
