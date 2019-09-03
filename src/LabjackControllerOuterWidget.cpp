#include <Wt/WApplication.h>
#include "LabjackControllerOuterWidget.h"
#include "TimeSeriesChart.h"
#include "WebAppHelpWidget.h"
#include "DataServerEvent.h"

using namespace Wt;

LabjackControllerOuterWidget::LabjackControllerOuterWidget(BoxControllerWebDataServer& server) : WContainerWidget(), server_(server), loggedIn_(false)
{
	this->appName = "Labjack Manager Overview:";
	this->addWidget(cpp14::make_unique<WText>(this->appName));

#if ENABLE_WEB_SERVER_LIVE_WIDGET
	this->labjackExampleWidget = this->addWidget(cpp14::make_unique<LabjackLiveStateWidget>());
#endif // ENABLE_WEB_SERVER_LIVE_WIDGET

	this->setPadding(10);
	this->resize(WLength::Auto, WLength::Auto);

	// Timeseries Charts:
	this->timeSeriesChartWidget = this->addWidget(cpp14::make_unique<TimeSeriesChart>());

	// Help Panel:
	this->webAppHelpWidget = this->addWidget(cpp14::make_unique<WebAppHelpWidget>());

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
	if (server_.connect(this, std::bind(&LabjackControllerOuterWidget::processDataServerEvent, this, std::placeholders::_1)))
		Wt::WApplication::instance()->enableUpdates(true);
}

void LabjackControllerOuterWidget::disconnect()
{
	if (server_.disconnect(this))
		Wt::WApplication::instance()->enableUpdates(false);
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
		cout << "LabjackControllerOuterWidget::processDataServerEvent(...): Historical data refreshed." << endl;
		auto historicalEvent = event.historicalDataLoadingEvent();
		// Update the time series chart widget
		this->timeSeriesChartWidget->processHistoricalDataUpdateEvent(historicalEvent);
	}
	else {
		cout << "Warning: DataServerEvent Event recieved but not yet implemented!" << endl;
	}
	
	///*
	// * This is the server call: we (schedule to) propagate the updated UI to
	// * the client.
	// *
	// * This schedules an update and returns immediately
	// */
	app->triggerUpdate();

	//newMessage();

	///*
	// * Anything else doesn't matter if we are not logged in.
	// */
	//if (!loggedIn())
	//	return;

	//bool display = event.type() != DataServerEvent::Message
	//	|| !userList_
	//	|| (users_.find(event.user()) != users_.end() && users_[event.user()]);

	//if (display) {
	//	Wt::WText* w = messages_->addWidget(Wt::cpp14::make_unique<Wt::WText>());

	//	/*
	//	 * If it fails, it is because the content wasn't valid XHTML
	//	 */
	//	if (!w->setText(event.formattedHTML(user_, Wt::TextFormat::XHTML))) {
	//		w->setText(event.formattedHTML(user_, Wt::TextFormat::Plain));
	//		w->setTextFormat(Wt::TextFormat::XHTML);
	//	}

	//	w->setInline(false);
	//	w->setStyleClass("chat-msg");

	//	/*
	//	 * Leave no more than 100 messages in the back-log
	//	 */
	//	if (messages_->count() > 100)
	//		messages_->removeChild(messages_->children()[0]);

	//	/*
	//	 * Little javascript trick to make sure we scroll along with new content
	//	 */
	//	app->doJavaScript(messages_->jsRef() + ".scrollTop += "
	//		+ messages_->jsRef() + ".scrollHeight;");

	//	/* If this message belongs to another user, play a received sound */
	//	if (event.user() != user_ && messageReceived_)
	//		messageReceived_->play();
	//}
}

