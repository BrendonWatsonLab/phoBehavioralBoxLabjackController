#include <map>

#include "TimeSeriesChart.h"
#include "ChartConfig.h"
#include "CsvUtil.h"

#include <Wt/WTime.h>
#include <Wt/WDate.h>
#include <Wt/WDateTime.h>
#include <Wt/WLocalDateTime.h>
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
#include "BehavioralBoxHistoricalData.h"




TimeSeriesChart::TimeSeriesChart() : Wt::WContainerWidget()
{
	this->addWidget(cpp14::make_unique<Wt::WText>(Wt::WString("Historic Labjack Data:")));
	this->setupLoadingIndicator();
}

void TimeSeriesChart::reload(std::vector<BehavioralBoxHistoricalData> historicalData)
{
	// Update the model
	this->model = this->buildHistoricDataModel(historicalData);
	if (!this->model)
		return;

	if (this->model->columnCount() <= 0) {
		return;
	}

	this->changeLoadingIndicatorVisibility(!this->isLoadingIndicatorVisible);

	//this->loadingContainerWidget->propagateSetVisible(false);

	/*
	 * Parses the first column as dates, to be able to use a date scale
	 */
	//if (this->shouldUseDateXAxis) {
	//	for (int i = 0; i < this->model->rowCount(); ++i) {			
	//		cpp17::any currData = model->data(i, 0);
	//		Wt::WString s = Wt::asString(currData);
	//		std::string::size_type sz = 0;   // alias of size_t
	//		unsigned long long currTimestampData = stoull(s, &sz);
	//		std::chrono::time_point<Clock> currDataTimepoint = LabjackHelpers::date_from_milliseconds_since_epoch(currTimestampData);
	//		//TODO: I think I need to use WLocalDateTime instead of WDateTime, as WDateTime assumes UTC
	//		Wt::WDateTime currTimestampDateTime = Wt::WDateTime(currDataTimepoint + std::chrono::hours(-4)); // Places the absolute center of the bar here:
	//		// Need to shift foward by 12 hours
	//		//Wt::WLocalDateTime currLocalTimestampDateTime = currTimestampDateTime.toLocalTime();
	//		model->setData(i, 0, currTimestampDateTime);
	//		//model->setData(i, 0, currLocalTimestampDateTime);
	//	}
	//}

	/*
	 * Build the data table.
	 */
	this->setupTable(this->model);

	/*
	 * Build the graphs.
	 */
	this->setupCharts(this->model);

	//this->addWidget(cpp14::make_unique<ChartConfig>(chart));

}


// GUI Setup:
void TimeSeriesChart::setupLoadingIndicator()
{
	this->loadingContainerWidget = this->addWidget(cpp14::make_unique<WContainerWidget>());
	this->loadingContainerWidget->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right); // center horizontally
	this->loadingContainerWidget->addWidget(cpp14::make_unique<Wt::WText>(Wt::WString("Loading Labjack Data...")));

}

void TimeSeriesChart::setupTable(const std::shared_ptr<WAbstractItemModel> model)
{
	// Show a view that allows editing of the model.
	this->tableContainerWidget = this->addWidget(cpp14::make_unique<Wt::WContainerWidget>());
	auto* table = this->tableContainerWidget->addWidget(cpp14::make_unique<Wt::WTableView>());

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
		int variableIndex = i - 1; // Subtract the time index
		bool isVariableAggregate = this->isVariableAggregate_[variableIndex];
		bool isColumnHidden = false;

		if (isVariableAggregate && (!this->tableDisplayOptions.shouldShowAggregateData)) {
			// if it is an aggregate variable and we're not supposed to show aggregate variables, it's hidden
			isColumnHidden = true;
		}
		//else if (!isVariableAggregate && (!this->tableDisplayOptions.shouldShowEventData)) {
		//	// if it's not an aggregate variable (meaning it's an event variable) and we're not supposed to show event data, it's hidden
		//	isColumnHidden = true;
		//}
		table->setColumnHidden(i, isColumnHidden);

		//table->setColumnWidth(i, Wt::WLength::Auto);
		table->setColumnWidth(i, 90);
		table->setColumnAlignment(i, Wt::AlignmentFlag::Center);
		table->setHeaderAlignment(i, Wt::AlignmentFlag::Center);
	}
}

void TimeSeriesChart::setupCharts(const std::shared_ptr<Wt::WAbstractItemModel> model)
{
	this->chartsContainerWidget = this->addWidget(cpp14::make_unique<WContainerWidget>());
	// Build the data series
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> dataSeries = this->buildDataSeries(model);
	
	// Iterate through and create each desired subplot
	for (int subplotIndex = 0; subplotIndex < this->getNumSubplots(); subplotIndex++) {
		// Make a container to hold the subplot and its related controls
		auto currSubplotContainer = this->chartsContainerWidget->addWidget(cpp14::make_unique<WContainerWidget>());
		currSubplotContainer->setMargin(Wt::WLength::Auto, Wt::Side::Left | Wt::Side::Right); // center horizontally

		Wt::Chart::WCartesianChart* currChart = currSubplotContainer->addWidget(cpp14::make_unique<Wt::Chart::WCartesianChart>());
		//chart->setPreferredMethod(WPaintedWidget::PngImage);
		//chart->setBackground(gray);
		currChart->setModel(model);        // set the model
		currChart->setXSeriesColumn(0);    // set the column that holds the X data
		currChart->setLegendEnabled(true); // enable the legend
		currChart->setZoomEnabled(true);
		currChart->setPanEnabled(true);
		
		//chart->axis(Wt::Chart::Axis::X).setVisible(false);
		//type: Bar
		// Marker: Inverted Triangle
		currChart->setType(Wt::Chart::ChartType::Scatter);            // set type to ScatterPlot

		///// SETUP X-AXIS:
		if (this->shouldUseDateXAxis) {
			currChart->axis(Wt::Chart::Axis::X).setScale(Wt::Chart::AxisScale::DateTime); // set scale of X axis to DateScale
			// Set the range appropriately:
			// Get the current datetime
			// Set the time to midnight
			// Add one day (to get the start of tomorrow)
			// Compute the datetime the desired number of days ago.
			// Convert both datetime values to milliseconds since unix epoch.
			// Set the chart's range.
			auto currRange = this->getCurrentDesiredRangeMillis();
			double divisionFactor = 1000.0;
			double labelInterval = double(TimeSeriesChart::millisPerDay) / divisionFactor;
			double earliest = double(currRange.start) / divisionFactor;
			double latest = double(currRange.end) / divisionFactor;
			currChart->axis(Wt::Chart::Axis::X).setRange(earliest, latest);
			// Set the label interval/spacing
			currChart->axis(Wt::Chart::Axis::X).setLabelInterval(labelInterval);
			// Set label to only display the date
			currChart->axis(Wt::Chart::Axis::X).setLabelFormat(WString::fromUTF8("MM/dd"));
			//TODO: Enable minutes and seconds for the ticks WString::fromUTF8("dd/MM hh:mm:ss");
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
		if (this->shouldEnableAggregateStatistics_) {
			if (this->shouldEnableSynchronize_Y_Axis) {
				// Set the current chart maximum to the global y-axis maximum so they're on a common scale.
				currChart->axis(Wt::Chart::Axis::Y).setMaximum(this->shared_y_axis_max);
			}
			else {
				// Otherwise set the current chart maximum to the maximum of it's displayed series
				currChart->axis(Wt::Chart::Axis::Y).setAutoLimits(Chart::AxisValue::Maximum);
			}
			
			currChart->axis(Wt::Chart::Axis::Y).setVisible(true);
		}
		else {
			currChart->axis(Wt::Chart::Axis::Y).setMaximum(10.0);
			currChart->axis(Wt::Chart::Axis::Y).setVisible(false);
		}

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
		int numOfDataSeriesInSubplot = this->subplotDataSeriesIndicies_[subplotIndex].size();
		for(int subplotDataSeriesIndex = 0; subplotDataSeriesIndex < numOfDataSeriesInSubplot; subplotDataSeriesIndex++) {
		//for each (int aSeriesIndex in this->subplotDataSeriesIndicies_[subplotIndex])
			int aSeriesIndex = this->subplotDataSeriesIndicies_[subplotIndex][subplotDataSeriesIndex];
			double aSeriesHeight = this->subplotDataSeriesHeights_[subplotIndex][subplotDataSeriesIndex];

		 // Add each series index in the array to the chart.
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


// Update function
void TimeSeriesChart::processHistoricalDataUpdateEvent(const HistoricalDataLoadingEvent& event)
{
	cout << "TimeSeriesChart::processHistoricalDataUpdateEvent(...):" << endl;
	if (event.type() == HistoricalDataLoadingEvent::Complete) {
		std::vector<BehavioralBoxHistoricalData> loadedHistoricalDataVect = event.dataLoadedHistoricalDataVector();
		cout << "processHistoricalDataUpdateEvent: complete event! Loaded " << loadedHistoricalDataVect.size() << " items." << endl;
		cout << "reloading.... " << endl;
		this->reload(loadedHistoricalDataVect);
		cout << "done." << endl;
	}
	else {
		cout << "WARNING: processHistoricalDataUpdateEvent(...): unimplemented event type!" << endl;
	}
}

void TimeSeriesChart::changeLoadingIndicatorVisibility(bool shouldLoadingIndicatorBeVisible)
{
	this->loadingContainerWidget->setHidden(!shouldLoadingIndicatorBeVisible);
	this->isLoadingIndicatorVisible = shouldLoadingIndicatorBeVisible;
}

// Builds a model from a vector of historical data
std::shared_ptr<Wt::WStandardItemModel> TimeSeriesChart::buildHistoricDataModel(std::vector<BehavioralBoxHistoricalData> historicalData)
{
	this->shared_y_axis_max = 0.0;

	if (historicalData.empty()) {
		cout << "WARNING: Data model empty!" << endl;
		return std::make_shared<Wt::WStandardItemModel>(0, 0); // Add one to numVariables to account for the timestamp column
	}

	//Hardcoded to get the first historicalData item passed
	BehavioralBoxHistoricalData activeHistoricalData = historicalData[0];
	int numVariables = activeHistoricalData.getNumberVariables();
	//TODO: get the number of distinct timestamps and then use those as the number of events instead of just using the max number of events. Each row must have a distinct timestamp, and there are no other requirements.
	//TODO: this is really inefficient because this is the format it starts out in and then I parse them into vectors of events for each variable.
	int maxNumEvents = activeHistoricalData.getMaxNumberEvents();
	int maxNumEventTimepoints = activeHistoricalData.getNumberOfUniqueTimepoints();

	std::vector<std::string> headerLabels = globalLabjackInputPortPurpose;

	// Aggregate functions and stuff
	EventStatistics activeEventStatistics = activeHistoricalData.getEventStatistics();
	this->shared_y_axis_max = activeEventStatistics.globalMaxNumEventsPerDay;
	int numStatisticsVariables = 0;
	int numAggregateEvents = activeEventStatistics.globalNumOfDays;
	int numColumns = (1 + numVariables); // Add one to numVariables to account for the timestamp column
	if (this->shouldEnableAggregateStatistics_) {
		numStatisticsVariables = activeEventStatistics.variableStatsVectors.size();
		numColumns += numStatisticsVariables;
		std::vector<std::string> statsAggLabels;
		for (int i = 0; i < headerLabels.size(); i++)
		{
			std::string statsAggLabel = headerLabels[i] + "_DayCount";
			statsAggLabels.push_back(statsAggLabel);

		}
		// Append the stats header labels to the end of the header labels array
		headerLabels.insert(std::end(headerLabels), std::begin(statsAggLabels), std::end(statsAggLabels));
	}
	// Add "time" variable to front of list
	headerLabels.insert(headerLabels.begin(), "time");


	int totalNumRows = maxNumEventTimepoints + numAggregateEvents;
	std::shared_ptr<Wt::WStandardItemModel> model = std::make_shared<Wt::WStandardItemModel>(totalNumRows, numColumns);

	// variables for loop
	int variableIndex = 0;
	for (variableIndex = 0; variableIndex < numVariables; variableIndex++)
	{
		std::vector<ParsedVariableEventType> historicalEvents = activeHistoricalData.getEvents(variableIndex);
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

		// Get the height of the data bar
		double currItemHeight = double(to_underlying(this->variableKindVect_[variableIndex]));

		// Iterate through the events for the given variable
		for (unsigned i = 0; i < currVarNumEvents; ++i) {
			ParsedVariableEventType currEvent = historicalEvents[i];
			int activeTimestampRowIndex = activeHistoricalData.getIndexForTimepoint(currEvent.datetime);

			unsigned long long x;
			if (this->shouldUseDateXAxis) {
				x = currEvent.milliseconds_since_epoch;
				Wt::WDateTime currTimestampDateTime = TimeSeriesChart::getCurrentLocalDateTimeFromMillis(x);
				model->setData(activeTimestampRowIndex, 0, currTimestampDateTime);
			}
			else {
				// Compute the relative (from the first timestamp) if we aren't using the date axis
				x = currEvent.milliseconds_since_epoch - earliest_event_timestamp;
				model->setData(activeTimestampRowIndex, 0, x); //TODO: i is being overwritten for different variables
			}

			model->setData(activeTimestampRowIndex, (variableIndex + 1), currItemHeight);
		}
	}

	// Stats (aggregate) variables for loop
	auto dayEventsCountsVect = activeEventStatistics.dayEventsCountsVect;
	int rowIndex = 0;
	for (const auto& anAggregateDayPair : dayEventsCountsVect) {

		// Build the datetimes
		Wt::WDateTime currTimestampDateTime = TimeSeriesChart::convertGMTTimePointToLocalDatetime(anAggregateDayPair.first);
		currTimestampDateTime = TimeSeriesChart::convertDateTimeToBarCenteredDatetime(currTimestampDateTime);
		model->setData(rowIndex, 0, currTimestampDateTime);
		
		for (int statsVariableIndex = 0; statsVariableIndex < numStatisticsVariables; statsVariableIndex++)
		{
			int absoluteColumnIndex = (variableIndex + 1) + statsVariableIndex;
			int currVarNumDays = anAggregateDayPair.second[statsVariableIndex];
			double currItemHeight = double(currVarNumDays);
			std::unique_ptr<NumericItem> prototype = cpp14::make_unique<NumericItem>();
			model->setItemPrototype(std::move(prototype));
			model->setData(rowIndex, absoluteColumnIndex, currItemHeight);
		} // end for each stats variable

		rowIndex++;
	}

	// Setup headers:
	for (int headerIndex = 0; headerIndex < headerLabels.size(); headerIndex++) {
		model->setHeaderData(headerIndex, WString(headerLabels[headerIndex]));
	}

	return model;
}

// Build vector of line series objects
std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> TimeSeriesChart::buildDataSeries(const std::shared_ptr<Wt::WAbstractItemModel> model)
{
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> outputVector;
	std::vector<Wt::WColor> colorVect = this->getVariableColors();
	/*
  * Add first two columns as line series
  */
	//int columnCount = this->getNumVariables() + 1; // Add one for the 0th column, the timestamp
	int columnCount = model->columnCount();
	for (int colIndex = 1; colIndex < columnCount; ++colIndex) {
		int currVariableIndex = colIndex - 1; // Subtract one to step back down to the variable indexing
		std::unique_ptr<Wt::Chart::WDataSeries> s = cpp14::make_unique<Wt::Chart::WDataSeries>(colIndex, Wt::Chart::SeriesType::Bar);
		Wt::WColor currVariableColor;
		if (currVariableIndex < colorVect.size()) {
			currVariableColor = colorVect[currVariableIndex];
		}
		else {
			currVariableColor = this->getDefaultColor();
		}
		Wt::WColor translucentCurrentColor = TimeSeriesChart::make_translucent(currVariableColor, 128);

		if (this->isVariableAggregate_[currVariableIndex]) { // aggregate statistics variable
			//s->setMarker(Wt::Chart::MarkerType::Square); // Make the series display upsidown triangles on top of the impulse plot bars
			//s->setType(Wt::Chart::SeriesType::Point); // Make the series display tall skinny bars, like an impulse plot
			//s->setLegendEnabled(true); // Disable the legend
			//s->setOffset(double(currVariableIndex) * 5.0); //isn't doing anything
			//const double desiredWidth = TimeSeriesChart::millisPerDay;
			s->setBarWidth(this->desiredAggregateBarWidth);

			//s->setScale()
			

			// Sets the label (text) colors:
			s->setLabelColor(currVariableColor);
			//s->setLabelsEnabled(Wt::Chart::Axis::X, true);
			s->setLabelsEnabled(Wt::Chart::Axis::Y, true);

			//// Sets the fill in the legend and the markers
			//Wt::WBrush fillBrush = std::move(currVariableColor);
			Wt::WBrush fillBrush = std::move(translucentCurrentColor);
			fillBrush.setStyle(Wt::BrushStyle::Solid);
			s->setBrush(fillBrush);

			//Wt::WPen mainPen = translucentCurrentColor;
			//mainPen.setCapStyle(Wt::PenCapStyle::Square);
			//mainPen.setJoinStyle(Wt::PenJoinStyle::Miter);
			//mainPen.setWidth(20.0);
			//s->setPen(mainPen);

		}
		else { // Regular impulse variable
			s->setMarker(Wt::Chart::MarkerType::InvertedTriangle); // Make the series display upsidown triangles on top of the impulse plot bars
			s->setType(Wt::Chart::SeriesType::Bar); // Make the series display tall skinny bars, like an impulse plot
			s->setLegendEnabled(true); // Disable the legend
			//s->setOffset(double(currVariableIndex) * 5.0); //isn't doing anything

			// Sets the label (text) colors:
			s->setLabelColor(currVariableColor); 

			// Sets the fill in the legend and the markers
			Wt::WBrush fillBrush = std::move(currVariableColor);
			//Wt::WBrush fillBrush = std::move(translucentCurrentColor);
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
			mainPen.setWidth(1.0);
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
		}

		outputVector.push_back(std::move(s));
	}
	return outputVector;
}

const Wt::WDateTime TimeSeriesChart::getCurrentLocalDateTimeFromMillis(unsigned long long millisSinceEpoch)
{
	std::chrono::time_point<Clock> currDataTimepoint = LabjackHelpers::date_from_milliseconds_since_epoch(millisSinceEpoch);
	Wt::WDateTime currTimestampDateTime = TimeSeriesChart::convertGMTTimePointToLocalDatetime(currDataTimepoint);
	return currTimestampDateTime;
}

const Wt::WDateTime TimeSeriesChart::convertGMTTimePointToLocalDatetime(std::chrono::system_clock::time_point GMTTimePoint)
{
	return Wt::WDateTime(GMTTimePoint + std::chrono::hours(TimeSeriesChart::currentTimezoneHoursOffsetFromGMT));
}

const Wt::WDateTime TimeSeriesChart::convertGMTDateTimeToLocalDatetime(Wt::WDateTime GMTDatetime)
{
	return GMTDatetime.addSecs(TimeSeriesChart::secondsPerHour * TimeSeriesChart::currentTimezoneHoursOffsetFromGMT);
}

const Wt::WDateTime TimeSeriesChart::convertDateTimeToBarCenteredDatetime(Wt::WDateTime unshiftedLocalDatetime)
{
	return unshiftedLocalDatetime.addSecs(TimeSeriesChart::secondsPerHour * 12);
}

TimeSeriesChart::CurrentDateTimeRange TimeSeriesChart::getCurrentDesiredRangeMillis()
{
	WDateTime currentDateTime = WDateTime::currentDateTime();
	//WLocalDateTime currentDateTime = WLocalDateTime::currentServerDateTime();
	Wt::WTime midnight = Wt::WTime(0, 0, 0, 0); // Midnight
	//TODAY:
	currentDateTime.setTime(midnight);
	// Compute tomorrow and the earliest range
	WDateTime tomorrowDateTime = currentDateTime.addDays(1);
	WDateTime earliestPlottedDateTime = tomorrowDateTime.addDays(-this->numDaysToDisplay_);
	unsigned long long earliest = LabjackHelpers::milliseconds_since_epoch_from_date(earliestPlottedDateTime.toTimePoint());
	unsigned long long latest = LabjackHelpers::milliseconds_since_epoch_from_date(tomorrowDateTime.toTimePoint());
	return CurrentDateTimeRange(earliest, latest);
}


