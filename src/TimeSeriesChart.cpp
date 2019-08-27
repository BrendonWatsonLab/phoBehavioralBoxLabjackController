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

TimeSeriesChart::TimeSeriesChart() : Wt::WContainerWidget()
{
	this->addWidget(cpp14::make_unique<Wt::WText>(Wt::WString::tr("scatter plot")));

	std::shared_ptr<WAbstractItemModel> model = this->buildHistoricDataModel();
	//std::shared_ptr<Wt::WStandardItemModel> model = this->buildHistoricDataModel();

	if (!model)
		return;

	/*
	 * Parses the first column as dates, to be able to use a date scale
	 */
	if (this->shouldUseDateXAxis) {
		for (int i = 0; i < model->rowCount(); ++i) {
			cpp17::any currData = model->data(i, 0);
			Wt::WString s = Wt::asString(currData);
			std::string::size_type sz = 0;   // alias of size_t
			unsigned long long currTimestampData = stoull(s, &sz);
			std::chrono::time_point<Clock> currDataTimepoint = LabjackHelpers::date_from_milliseconds_since_epoch(currTimestampData);
			Wt::WDateTime currTimestampDateTime = Wt::WDateTime(currDataTimepoint);
			model->setData(i, 0, currTimestampDateTime);
		}
	}
	// Show a view that allows editing of the model.
	auto* w = this->addWidget(cpp14::make_unique<Wt::WContainerWidget>());
	auto* table = w->addWidget(cpp14::make_unique<Wt::WTableView>());

	table->setMargin(10, Wt::Side::Top | Wt::Side::Bottom);
	table->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right);

	table->setModel(model);
	table->setSortingEnabled(false); // Does not make much sense for time series
	table->setColumnResizeEnabled(true);
	table->setSelectionMode(Wt::SelectionMode::None);
	table->setAlternatingRowColors(true);
	table->setColumnAlignment(0, Wt::AlignmentFlag::Center);
	table->setHeaderAlignment(0, Wt::AlignmentFlag::Center);
	table->setRowHeight(22);

	// Editing does not really work without Ajax, it would require an
	// additional button somewhere to confirm the edited value.
	if (Wt::WApplication::instance()->environment().ajax()) {
		table->resize(800, 20 + 5 * 22);
		table->setEditTriggers(Wt::EditTrigger::SingleClicked);
	}
	else {
		table->resize(800, 20 + 5 * 22 + 25);
		table->setEditTriggers(Wt::EditTrigger::None);
	}

	std::shared_ptr<Wt::WItemDelegate> delegate	= std::make_shared<Wt::WItemDelegate>();
	delegate->setTextFormat("%.1f");
	table->setItemDelegate(delegate);

	std::shared_ptr<Wt::WItemDelegate> delegateColumn = std::make_shared<Wt::WItemDelegate>();
	table->setItemDelegateForColumn(0, delegateColumn);

	table->setColumnWidth(0, 80);
	for (int i = 1; i < model->columnCount(); ++i) {
		table->setColumnWidth(i, Wt::WLength::Auto);
		//table->setColumnWidth(i, 90);
	}

	/*
	 * Create the scatter plot.
	 */
	Wt::Chart::WCartesianChart* chart = this->addWidget(cpp14::make_unique<Wt::Chart::WCartesianChart>());
	//chart->setPreferredMethod(WPaintedWidget::PngImage);
	//chart->setBackground(gray);
	chart->setModel(model);        // set the model
	chart->setXSeriesColumn(0);    // set the column that holds the X data
	chart->setLegendEnabled(true); // enable the legend
	chart->setZoomEnabled(true);
	chart->setPanEnabled(true);
	chart->axis(Wt::Chart::Axis::Y).setVisible(false);
	chart->axis(Wt::Chart::Axis::X).setVisible(false);

	//type: Bar
	// Marker: Inverted Triangle
	chart->setType(Wt::Chart::ChartType::Scatter);            // set type to ScatterPlot
	if (this->shouldUseDateXAxis) {
		chart->axis(Wt::Chart::Axis::X).setScale(Wt::Chart::AxisScale::DateTime); // set scale of X axis to DateScale
	}
	// Automatically layout chart (space for axes, legend, ...)
	chart->setAutoLayoutEnabled();

	chart->setBackground(Wt::WColor(200, 200, 200));
	/*
	  * Add first two columns as line series
	  */
	for (int i = 1; i < 3; ++i) {
		std::unique_ptr<Wt::Chart::WDataSeries> s	= cpp14::make_unique<Wt::Chart::WDataSeries>(i, Wt::Chart::SeriesType::Line);
		//s->setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));
		s->setMarker(Wt::Chart::MarkerType::InvertedTriangle); // Make the series display upsidown triangles on top of the impulse plot bars
		s->setType(Wt::Chart::SeriesType::Bar); // Make the series display tall skinny bars, like an impulse plot
		s->setLegendEnabled(false); // Disable the legend
		chart->addSeries(std::move(s));
	}

	chart->resize(800, 400); // WPaintedWidget must be given explicit size

	chart->setMargin(10, Wt::Side::Top | Wt::Side::Bottom);            // add margin vertically
	chart->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right); // center horizontally

	this->addWidget(cpp14::make_unique<ChartConfig>(chart));
}



std::shared_ptr<Wt::WStandardItemModel> TimeSeriesChart::buildHistoricDataModel()
{
	std::vector<BehavioralBoxHistoricalData> historicalData = BehavioralBoxControllersManager::loadHistoricalData();
	BehavioralBoxHistoricalData activeHistoricalData = historicalData[0];
	std::vector< std::pair<unsigned long long, double> > historicalEvents = activeHistoricalData.getEvents(3);

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
		//double x = (static_cast<double>(currEvent.first) - earliest_event_timestamp);
		//unsigned long long x = currEvent.first - earliest_event_timestamp;
		//double x = static_cast<double>(currEvent.first - earliest_event_timestamp);

		unsigned long long x;
		if (this->shouldUseDateXAxis) {
			x = currEvent.first;
		}
		else {
			// Compute the relative (from the first timestamp) if we aren't using the date axis
			x = currEvent.first - earliest_event_timestamp;
		}
		model->setData(i, 0, x);
		model->setData(i, 1, 10.0);
	}

	return model;
}

