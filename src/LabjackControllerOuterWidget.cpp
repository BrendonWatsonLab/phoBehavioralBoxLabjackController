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
#include "LabjackControllerOuterWidget.h"
#include "TimeSeriesChart.h"
#include "WebAppHelpWidget.h"
#include "DataServerEvent.h"

using namespace Wt;

LabjackControllerOuterWidget::LabjackControllerOuterWidget(BoxControllerWebDataServer& server) : WContainerWidget(), server_(server), loggedIn_(false)
{
	// TODO: Set appName to BBID
	// Gets the computer's hostname
	this->hostName = this->server_.getHostName();
	this->appName = "Labjack Manager Overview: " + this->hostName;
	
#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget = this->addWidget(cpp14::make_unique<LabjackLiveStateWidget>());
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET

	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);

	// Setup main layout
	mainLayout_ = this->setLayout(Wt::cpp14::make_unique<Wt::WVBoxLayout>());

	auto contentsStack = Wt::cpp14::make_unique<Wt::WStackedWidget>();
	contentsStack_ = contentsStack.get();

	// Timeseries Charts:
	this->timeSeriesChartWidget = this->contentsStack_->addWidget(cpp14::make_unique<TimeSeriesChart>());

	// Help Panel:
	this->webAppHelpWidget = this->contentsStack_->addWidget(cpp14::make_unique<WebAppHelpWidget>());

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

LabjackControllerOuterWidget::~LabjackControllerOuterWidget()
{
	if (loggedIn()) {
		loggedIn_ = false;
		disconnect();
	}
}

void LabjackControllerOuterWidget::connect()
{
	if (server_.connect(this, std::bind(&LabjackControllerOuterWidget::processDataServerEvent, this, std::placeholders::_1))) {
		Wt::WApplication::instance()->enableUpdates(true);
	}
}

void LabjackControllerOuterWidget::disconnect()
{
	if (server_.disconnect(this)) {
		Wt::WApplication::instance()->enableUpdates(false);
	}
}

void LabjackControllerOuterWidget::setActiveLabjacks(std::vector<BehavioralBoxLabjack*> activeLabjacks)
{
#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget->updateLabjacks(activeLabjacks);
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET	
}

void LabjackControllerOuterWidget::requestServerHistoricalData()
{
	this->server_.requestHistoricalData();
}

void LabjackControllerOuterWidget::requestServerHistoricalDataReload()
{
	this->server_.requestHistoricalDataReload();
}

bool LabjackControllerOuterWidget::loggedIn() const
{
	return loggedIn_;
}

void LabjackControllerOuterWidget::processDataServerEvent(const DataServerEvent& event)
{
	Wt::WApplication* app = Wt::WApplication::instance();

	/*
	 * This is where the "server-push" happens. The data server posts to this
	 * event from other sessions, see SimpleChatServer::postChatEvent()
	 */
	if (event.type() == DataServerEvent::Type::HistoricalDataRefreshed) {
		std::cout << "LabjackControllerOuterWidget::processDataServerEvent(...): Historical data refreshed." << std::endl;
		auto historicalEvent = event.historicalDataLoadingEvent();
		if (historicalEvent.type() == HistoricalDataLoadingEvent::Type::Complete) {
			this->loadedHistoricalDataVectIDs_.clear();
			std::vector<BehavioralBoxHistoricalData> loadedHistoricalDataVect = historicalEvent.dataLoadedHistoricalDataVector();
			for (size_t i = 0; i < loadedHistoricalDataVect.size(); i++)
			{
				std::string currBoxIdentifier = loadedHistoricalDataVect[i].getBoxIdentifier();
				this->loadedHistoricalDataVectIDs_.push_back(currBoxIdentifier);
			}
			this->updateActiveLabjackInfo();
			
		}
		// Update the time series chart widget
		this->timeSeriesChartWidget->processHistoricalDataUpdateEvent(historicalEvent);
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
	app->triggerUpdate();

}

void LabjackControllerOuterWidget::setupHeader()
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

	//Wt::WAnimation animation(Wt::AnimationEffect::Fade,	Wt::TimingFunction::Linear,	200);
	//contentsStack_->setTransitionAnimation(animation, true);

	/*
	 * Setup the top-level menu
	 */
	//auto menu = Wt::cpp14::make_unique<Wt::WMenu>(contentsStack_);
	//menu->setInternalPathEnabled();
	//menu->setInternalBasePath("/");

	//addToMenu(menu.get(), "Layout", Wt::cpp14::make_unique<Layout>());
	//addToMenu(menu.get(), "Forms", Wt::cpp14::make_unique<FormWidgets>());
	//addToMenu(menu.get(), "Navigation", Wt::cpp14::make_unique<Navigation>());
	//addToMenu(menu.get(), "Trees & Tables", Wt::cpp14::make_unique<TreesTables>())->setPathComponent("trees-tables");
	//addToMenu(menu.get(), "Graphics & Charts", Wt::cpp14::make_unique<GraphicsWidgets>())->setPathComponent("graphics-charts");
	////addToMenu(menu.get(), "Events", Wt::cpp14::make_unique<EventsDemo>());
	//addToMenu(menu.get(), "Media", Wt::cpp14::make_unique<Media>());

	//auto item = Wt::cpp14::make_unique<Wt::WMenuItem>("TEST");
	//auto item_ = menu->addItem(std::move(item));

	//item = Wt::cpp14::make_unique<Wt::WMenuItem>("TEST 2");
	//item_ = menu->addItem(std::move(item));

	//navigation_->addMenu(std::move(menu));

	auto inactiveActiveLabjackLabel = this->headerRootContainer_->addWidget(cpp14::make_unique<WText>("Active Labjack: "));
	this->activeLabjackName_ = this->headerRootContainer_->addWidget(cpp14::make_unique<WText>("None"));


	// Add it to the layout
	this->mainLayout_->addWidget(std::move(navigation), 0);
	this->mainLayout_->addWidget(std::move(headerRootContainer), 1);

}

void LabjackControllerOuterWidget::updateActiveLabjackInfo()
{
	if (this->loadedHistoricalDataVectIDs_.size() > 0) {
		this->activeLabjackName_->setText(this->loadedHistoricalDataVectIDs_[this->currActiveLabjackIndex_]);
	}
	else {
		this->activeLabjackName_->setText("None");
	}
}

