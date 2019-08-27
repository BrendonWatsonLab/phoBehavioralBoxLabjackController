#include "TimeSeriesChart.h"
#include "ChartConfig.h"
#include "CsvUtil.h"


#include <Wt/WDate.h>
#include <Wt/WEnvironment.h>
#include <Wt/WItemDelegate.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WText.h>

#include <Wt/WBorderLayout.h>
#include <Wt/WFitLayout.h>

#include <Wt/WStandardItem.h>
#include <Wt/WTableView.h>

#include <Wt/Chart/WCartesianChart.h>
#include <Wt/Chart/WPieChart.h>

#include "LabjackHelpers.h"
#include "BehavioralBoxControllersManager.h"
#include "NumericItem.h"

using namespace Wt;
using namespace Wt::Chart;

TimeSeriesChart::TimeSeriesChart() : Wt::WContainerWidget()
{
	this->addWidget(cpp14::make_unique<WText>(WString::tr("scatter plot")));

	std::shared_ptr<WAbstractItemModel> model = this->buildHistoricDataModel();

	if (!model)
		return;

	/*
	 * Parses the first column as dates, to be able to use a date scale
	 */
	//for (int i = 0; i < model->rowCount(); ++i) {
	//	WString s = asString(model->data(i, 0));
	//	wcstoull(s, )
	//	//auto currData = model->data(i, 0);
	//	/*unsigned long long currData = static_cast<unsigned long long>(model->data(i, 0));*/
	//	unsigned long long currData = static_cast<unsigned long long>(model->data(i, 0));
	//	std::chrono::time_point<Clock> currDataTimepoint = LabjackHelpers::date_from_milliseconds_since_epoch(currData);

	//	WDate d = WDate::fromString(s, "dd/MM/yy");
	//	model->setData(i, 0, d);
	//}

	// Show a view that allows editing of the model.
	auto* w = this->addWidget(cpp14::make_unique<WContainerWidget>());
	auto* table = w->addWidget(cpp14::make_unique<WTableView>());

	table->setMargin(10, Side::Top | Side::Bottom);
	table->setMargin(WLength::Auto, Side::Left | Side::Right);

	table->setModel(model);
	table->setSortingEnabled(false); // Does not make much sense for time series
	table->setColumnResizeEnabled(true);
	table->setSelectionMode(SelectionMode::None);
	table->setAlternatingRowColors(true);
	table->setColumnAlignment(0, AlignmentFlag::Center);
	table->setHeaderAlignment(0, AlignmentFlag::Center);
	table->setRowHeight(22);

	// Editing does not really work without Ajax, it would require an
	// additional button somewhere to confirm the edited value.
	if (WApplication::instance()->environment().ajax()) {
		table->resize(800, 20 + 5 * 22);
		table->setEditTriggers(EditTrigger::SingleClicked);
	}
	else {
		table->resize(800, 20 + 5 * 22 + 25);
		table->setEditTriggers(EditTrigger::None);
	}

	std::shared_ptr<WItemDelegate> delegate	= std::make_shared<WItemDelegate>();
	delegate->setTextFormat("%.1f");
	table->setItemDelegate(delegate);

	std::shared_ptr<WItemDelegate> delegateColumn = std::make_shared<WItemDelegate>();
	table->setItemDelegateForColumn(0, delegateColumn);

	table->setColumnWidth(0, 80);
	for (int i = 1; i < model->columnCount(); ++i)
		table->setColumnWidth(i, 90);

	/*
	 * Create the scatter plot.
	 */
	WCartesianChart* chart = this->addWidget(cpp14::make_unique<WCartesianChart>());
	//chart->setPreferredMethod(WPaintedWidget::PngImage);
	//chart->setBackground(gray);
	chart->setModel(model);        // set the model
	chart->setXSeriesColumn(0);    // set the column that holds the X data
	chart->setLegendEnabled(true); // enable the legend
	chart->setZoomEnabled(true);
	chart->setPanEnabled(true);

	chart->setType(ChartType::Scatter);            // set type to ScatterPlot
	//chart->axis(Axis::X).setScale(AxisScale::Date); // set scale of X axis to DateScale

	// Automatically layout chart (space for axes, legend, ...)
	chart->setAutoLayoutEnabled();

	chart->setBackground(WColor(200, 200, 200));
	/*
	  * Add first two columns as line series
	  */
	for (int i = 1; i < 3; ++i) {
		std::unique_ptr<WDataSeries> s	= cpp14::make_unique<WDataSeries>(i, SeriesType::Line);
		s->setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));
		chart->addSeries(std::move(s));
	}

	chart->resize(800, 400); // WPaintedWidget must be given explicit size

	chart->setMargin(10, Side::Top | Side::Bottom);            // add margin vertically
	chart->setMargin(WLength::Auto, Side::Left | Side::Right); // center horizontally

	this->addWidget(cpp14::make_unique<ChartConfig>(chart));
}

std::shared_ptr<Wt::WAbstractItemModel> TimeSeriesChart::buildHistoricDataModel()
{
	std::vector<BehavioralBoxHistoricalData> historicalData = BehavioralBoxControllersManager::loadHistoricalData();
	BehavioralBoxHistoricalData activeHistoricalData = historicalData[0];
	std::vector< std::pair<unsigned long long, double> > historicalEvents = activeHistoricalData.getEvents(0);

	// Sort the events by ascending timestamp
	sort(historicalEvents.begin(), historicalEvents.end());

	int numEvents = historicalEvents.size();
	unsigned long long earliest_event_timestamp = historicalEvents[0].first;
	//double earliest_event_timestamp_value = static_cast<double>(earliest_event_timestamp);
	//std::shared_ptr<WStandardItemModel> model = std::make_shared<WStandardItemModel>(0, 0);
	std::shared_ptr<Wt::WStandardItemModel> model = std::make_shared<Wt::WStandardItemModel>(numEvents, 2);
	std::unique_ptr<NumericItem> prototype = cpp14::make_unique<NumericItem>();
	model->setItemPrototype(std::move(prototype));
	model->setHeaderData(0, WString("X"));
	model->setHeaderData(1, WString("Event Value"));

	for (unsigned i = 0; i < numEvents; ++i) {
		std::pair<unsigned long long, double> currEvent = historicalEvents[i];

		/*double x = (static_cast<double>(i) - 20) / 4;*/
		double x = (static_cast<double>(currEvent.first) - earliest_event_timestamp);
		model->setData(i, 0, x);
		model->setData(i, 1, 10.0);
	}

	return model;
}

