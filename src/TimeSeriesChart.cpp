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

#include <Wt/Chart/WAxisSliderWidget.h>
#include <Wt/Chart/WCartesianChart.h>
#include <Wt/Chart/WDataSeries.h>

#include "LabjackHelpers.h"
#include "BehavioralBoxControllersManager.h"
#include "NumericItem.h"

#define TIME_SERIES_CHART_NUM_TABLE_ROWS_SHOWN 8
#define TIME_SERIES_CHART_NUM_TABLE_ROW_HEIGHT 26
#define TIME_SERIES_CHART_SUBPLOT_HEIGHT 230
#define TIME_SERIES_CHART_SUBPLOT_WIDTH 1080
#define TIME_SERIES_CHART_RANGE_SLIDER_HEIGHT 80


TimeSeriesChart::TimeSeriesChart() : Wt::WContainerWidget()
{
	this->addWidget(cpp14::make_unique<Wt::WText>(Wt::WString("Historic Labjack Data:")));

	std::shared_ptr<WAbstractItemModel> model = this->buildHistoricDataModel();
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
	
	/*
	 * Build the data table.
	 */
	this->setupTable(model);


	/*
	 * Build the graphs.
	 */
	this->setupCharts(model);


	//this->addWidget(cpp14::make_unique<ChartConfig>(chart));
}



std::shared_ptr<Wt::WStandardItemModel> TimeSeriesChart::buildHistoricDataModel()
{
	std::vector<BehavioralBoxHistoricalData> historicalData = BehavioralBoxControllersManager::loadHistoricalData();
	BehavioralBoxHistoricalData activeHistoricalData = historicalData[0];
	int numVariables = activeHistoricalData.getNumberVariables();
	int maxNumEvents = activeHistoricalData.getMaxNumberEvents();
	std::shared_ptr<Wt::WStandardItemModel> model = std::make_shared<Wt::WStandardItemModel>(maxNumEvents, (1 + numVariables)); // Add one to numVariables to account for the timestamp column

	/*std::vector<std::string> headerLabels = activeHistoricalData.getHeaderLabels();*/

	//auto headerLabels = activeHistoricalData.getHeaderLabels();
	std::vector<std::string> headerLabels = globalLabjackInputPortPurpose;
	// Add "time" variable to front of list
	headerLabels.insert(headerLabels.begin(), "time");
	

	//verticalVariableSeparatorMultiplier: the vertical separation between the variables on the graph
	double verticalVariableSeparatorMultiplier = 1.0;

	for (int variableIndex = 0; variableIndex < numVariables; variableIndex++)
	{	
		std::vector< ParsedVariableEventType > historicalEvents = activeHistoricalData.getEvents(variableIndex);
		int currVarNumEvents = historicalEvents.size();

		// Sort the events by ascending timestamp
		unsigned long long earliest_event_timestamp = 0;
		if (!this->shouldUseDateXAxis) {
			sort(historicalEvents.begin(), historicalEvents.end());
			// Compute earliest timestamp if we're in relative (not absolute date axis) mode.
			earliest_event_timestamp = historicalEvents[0].milliseconds_since_epoch;
		}
		std::unique_ptr<NumericItem> prototype = cpp14::make_unique<NumericItem>();
		model->setItemPrototype(std::move(prototype));
		// Iterate through the events for the given variable
		for (unsigned i = 0; i < currVarNumEvents; ++i) {
			ParsedVariableEventType currEvent = historicalEvents[i];
			unsigned long long x;
			if (this->shouldUseDateXAxis) {
				x = currEvent.milliseconds_since_epoch;
			}
			else {
				// Compute the relative (from the first timestamp) if we aren't using the date axis
				x = currEvent.milliseconds_since_epoch - earliest_event_timestamp;
			}
			model->setData(i, 0, x);
			model->setData(i, (variableIndex+1), (double(variableIndex) * verticalVariableSeparatorMultiplier));
		}
	}


	// Setup headers:
	for (int headerIndex = 0; headerIndex < headerLabels.size(); headerIndex++) {
		model->setHeaderData(headerIndex, WString(headerLabels[headerIndex]));
	}

	return model;
}

void TimeSeriesChart::setupTable(const std::shared_ptr<WAbstractItemModel> model)
{
	// Show a view that allows editing of the model.
	auto* w = this->addWidget(cpp14::make_unique<Wt::WContainerWidget>());
	auto* table = w->addWidget(cpp14::make_unique<Wt::WTableView>());

	table->setMargin(10, Wt::Side::Top | Wt::Side::Bottom);
	table->setMargin(10, Wt::Side::Left | Wt::Side::Right);

	table->setModel(model);
	table->setSortingEnabled(false); // Does not make much sense for time series
	table->setColumnResizeEnabled(false);
	table->setSelectionMode(Wt::SelectionMode::None);
	table->setAlternatingRowColors(true);
	table->setRowHeight(TIME_SERIES_CHART_NUM_TABLE_ROW_HEIGHT);

	// Editing does not really work without Ajax, it would require an
	// additional button somewhere to confirm the edited value.
	if (Wt::WApplication::instance()->environment().ajax()) {
		table->resize(1024, 20 + TIME_SERIES_CHART_NUM_TABLE_ROWS_SHOWN * TIME_SERIES_CHART_NUM_TABLE_ROW_HEIGHT);
		table->setEditTriggers(Wt::EditTrigger::SingleClicked);
	}
	else {
		table->resize(1024, 20 + TIME_SERIES_CHART_NUM_TABLE_ROWS_SHOWN * TIME_SERIES_CHART_NUM_TABLE_ROW_HEIGHT + 25);
		table->setEditTriggers(Wt::EditTrigger::None);
	}

	std::shared_ptr<Wt::WItemDelegate> delegate = std::make_shared<Wt::WItemDelegate>();
	delegate->setTextFormat("%.1f");
	table->setItemDelegate(delegate);

	std::shared_ptr<Wt::WItemDelegate> delegateColumn = std::make_shared<Wt::WItemDelegate>();
	table->setItemDelegateForColumn(0, delegateColumn);

	table->setColumnWidth(0, 140);
	table->setColumnAlignment(0, Wt::AlignmentFlag::Left);
	table->setHeaderAlignment(0, Wt::AlignmentFlag::Left);
	for (int i = 1; i < model->columnCount(); ++i) {
		//table->setColumnWidth(i, Wt::WLength::Auto);
		table->setColumnWidth(i, 90);
		table->setColumnAlignment(i, Wt::AlignmentFlag::Center);
		table->setHeaderAlignment(i, Wt::AlignmentFlag::Center);
	}
}

void TimeSeriesChart::setupCharts(const std::shared_ptr<Wt::WAbstractItemModel> model)
{
	// Build the data series
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> dataSeries = this->buildDataSeries(model);
	int numSubplots = 5; // 4 food/water signals + 1 for running wheel

	// the data series to include on a given subplot
	std::vector<std::vector<int>> subplotDataSeriesIndicies = { {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8} };

	std::vector<Wt::WColor> colorVect = this->getVariableColors();
	// Iterate through and create each desired subplot
	for (int subplotIndex = 0; subplotIndex < numSubplots; subplotIndex++) {
		// Make a container to hold the subplot and its related controls
		auto currSubplotContainer = this->addWidget(cpp14::make_unique<WContainerWidget>());
		currSubplotContainer->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right); // center horizontally

		Wt::Chart::WCartesianChart* currChart = this->addWidget(cpp14::make_unique<Wt::Chart::WCartesianChart>());
		//Wt::Chart::WCartesianChart* currChart = currSubplotContainer->addWidget(cpp14::make_unique<Wt::Chart::WCartesianChart>());
		//chart->setPreferredMethod(WPaintedWidget::PngImage);
		//chart->setBackground(gray);
		currChart->setModel(model);        // set the model
		currChart->setXSeriesColumn(0);    // set the column that holds the X data
		currChart->setLegendEnabled(true); // enable the legend
		currChart->setZoomEnabled(true);
		currChart->setPanEnabled(true);
		currChart->setCrosshairEnabled(true);
		currChart->axis(Wt::Chart::Axis::Y).setVisible(false);
		//chart->axis(Wt::Chart::Axis::X).setVisible(false);
		//type: Bar
		// Marker: Inverted Triangle
		currChart->setType(Wt::Chart::ChartType::Scatter);            // set type to ScatterPlot

		///// SETUP X-AXIS:
		if (this->shouldUseDateXAxis) {
			currChart->axis(Wt::Chart::Axis::X).setScale(Wt::Chart::AxisScale::DateTime); // set scale of X axis to DateScale
		}
		currChart->axis(Wt::Chart::Axis::X).setGridLinesEnabled(true);
		//Wt::Chart::AxisConfig xAxisConfig = currChart->axis(Chart::Axis::X).config

		//currChart->axis(Chart::Axis::X).gridLinePositions()
		// Set maximum X zoom level to 16x zoom
		double min = asNumber(model->data(0, 0));
		double max = asNumber(model->data(model->rowCount() - 1, 0));
		currChart->axis(Chart::Axis::X).setMinimumZoomRange((max - min) / 16.0);

		///// SETUP Y-AXIS:
		currChart->axis(Wt::Chart::Axis::Y).setScale(Wt::Chart::AxisScale::Linear);
		currChart->axis(Wt::Chart::Axis::Y).setLocation(Chart::AxisValue::Zero);
		currChart->axis(Wt::Chart::Axis::Y).setMinimum(0.0);
		currChart->axis(Wt::Chart::Axis::Y).setMaximum(model->columnCount());

		// Set up the User Interaction Behavior:
		/*Wt::Chart::WheelActions newWheelActions = Wt::Chart::WheelActions();*/
		Wt::Chart::WheelActions chartInteractionMouseWheelActions = currChart->wheelActions();
		chartInteractionMouseWheelActions[KeyboardModifier::None] = InteractiveAction::PanX;
		chartInteractionMouseWheelActions[WFlags<KeyboardModifier>(KeyboardModifier::Alt) |	KeyboardModifier::Control] = InteractiveAction::ZoomX;
		chartInteractionMouseWheelActions[WFlags<KeyboardModifier>(KeyboardModifier::Control) |	KeyboardModifier::Shift] = InteractiveAction::ZoomX;
		chartInteractionMouseWheelActions[KeyboardModifier::Control] = InteractiveAction::ZoomX;
		chartInteractionMouseWheelActions[WFlags<KeyboardModifier>(KeyboardModifier::Alt) |	KeyboardModifier::Control |	KeyboardModifier::Shift] = InteractiveAction::ZoomX;
		currChart->setWheelActions(chartInteractionMouseWheelActions);


		// Automatically layout chart (space for axes, legend, ...)
		currChart->setAutoLayoutEnabled();
		currChart->setBackground(Wt::WColor(210, 210, 210));
	
		// Data series:
		bool isFirstSeriesForSubplot = true;
		for each (int aSeriesIndex in subplotDataSeriesIndicies[subplotIndex])
		{ // Add each series index in the array to the chart.
			//auto currSeriesPointer_ = dataSeries[aSeriesIndex].get();
			currChart->addSeries(std::move(dataSeries[aSeriesIndex]));
			// Add WAxisSliderWidget:
			//if (isFirstSeriesForSubplot) {				
			//	// Add a WAxisSliderWidget for the chart using the first data series
			//	Wt::Chart::WAxisSliderWidget* sliderWidget = this->addWidget(cpp14::make_unique<Wt::Chart::WAxisSliderWidget>());
			//	sliderWidget->setSeries(currSeriesPointer_);
			//	//currChart->addAxisSliderWidget(sliderWidget);

			//	//auto sliderWidget = currSubplotContainer->addWidget(cpp14::make_unique<Chart::WAxisSliderWidget>(currSeriesPointer_));
			//	sliderWidget->resize(TIME_SERIES_CHART_SUBPLOT_WIDTH, TIME_SERIES_CHART_RANGE_SLIDER_HEIGHT);
			//	//sliderWidget->setSelectionAreaPadding(40, Side::Left | Side::Right);
			//	sliderWidget->setBackground(Wt::WBrush(Wt::WColor(255, 0, 0, 255)));
			//	//sliderWidget->setMargin(2, Wt::Side::Top | Wt::Side::Bottom); // remove margin vertically
			//	//sliderWidget->setMargin(WLength::Auto, Side::Left | Side::Right); // Center horizontally
			//	//isFirstSeriesForSubplot = false; // Indicate that any series following this one are not the first series
			//}
			
		}
		
		currChart->resize(TIME_SERIES_CHART_SUBPLOT_WIDTH, TIME_SERIES_CHART_SUBPLOT_HEIGHT); // WPaintedWidget must be given explicit size

		currChart->setMargin(0, Wt::Side::Top | Wt::Side::Bottom); // add margin vertically
		currChart->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right); // center horizontally
	}



	


	
	
}


// Build vector of line series objects
std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> TimeSeriesChart::buildDataSeries(const std::shared_ptr<Wt::WAbstractItemModel> model)
{
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> outputVector;
	std::vector<Wt::WColor> colorVect = this->getVariableColors();
	/*
  * Add first two columns as line series
  */
	for (int colIndex = 1; colIndex < model->columnCount(); ++colIndex) {
		int currVariableIndex = colIndex - 1;
		std::unique_ptr<Wt::Chart::WDataSeries> s = cpp14::make_unique<Wt::Chart::WDataSeries>(colIndex, Wt::Chart::SeriesType::Bar);
		s->setMarker(Wt::Chart::MarkerType::InvertedTriangle); // Make the series display upsidown triangles on top of the impulse plot bars
		s->setType(Wt::Chart::SeriesType::Bar); // Make the series display tall skinny bars, like an impulse plot
		s->setLegendEnabled(true); // Disable the legend
		//s->setOffset(double(currVariableIndex) * 5.0); //isn't doing anything

		Wt::WColor currVariableColor;
		if (currVariableIndex < colorVect.size()) {
			currVariableColor = colorVect[currVariableIndex];
		}
		else {
			currVariableColor = this->getDefaultColor();
		}
		Wt::WColor translucentCurrentColor = Wt::WColor(currVariableColor.red(), currVariableColor.green(), currVariableColor.blue(), 128);
		// Sets the label (text) colors:
		s->setLabelColor(currVariableColor); // Subtract one to step back down to the variable indexing

		// Sets the fill in the legend and the markers
		Wt::WBrush fillBrush = std::move(currVariableColor);
		fillBrush.setStyle(Wt::BrushStyle::Solid);
		s->setBrush(fillBrush);
		//s->setBrush(Wt::WBrush(currVariableColor));

		// Sets the shadow colors:
		//s->setShadow(WShadow(2, 2, currVariableColor, 3));
		//s->setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));

		//Wt::WPen markerPen = s->markerPen();
		//markerPen.setColor(currVariableColor);
		//s->setMarkerPen(Wt::WPen(currVariableColor));
		//s->setMarkerBrush(Wt::WBrush(currVariableColor));

		//Wt::WPen mainPen = s->pen();
		//mainPen.setColor(currVariableColor);

		// Sets the lines
		//Wt::WPen mainPen = Wt::WColor(0, 191, 255);

		Wt::WPen mainPen = translucentCurrentColor;
		mainPen.setCapStyle(Wt::PenCapStyle::Round);
		mainPen.setJoinStyle(Wt::PenJoinStyle::Round);
		mainPen.setWidth(3.0);
		s->setPen(mainPen);
		//s->setPen(Wt::WPen(currVariableColor));

		//Wt::WBrush existingBrush = s->brush();
		//existingBrush.setColor(currVariableColor);
		//s->setBrush(existingBrush);
		//Wt::WBrush existingBrush = s->brush();
		//existingBrush.setColor(Wt::WColor(Wt::StandardColor::Blue));
		//s->setBrush(existingBrush);
		//s->setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::Blue)));

		//s->setLegendEnabled(false); // Disable the legend
		//currChart->addSeries(std::move(s));
		outputVector.push_back(std::move(s));
	}
	return outputVector;
}


