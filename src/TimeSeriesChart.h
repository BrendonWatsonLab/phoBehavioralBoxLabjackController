#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

class TimeSeriesChart : public Wt::WContainerWidget
{
public:
	TimeSeriesChart();

	bool shouldUseDateXAxis = true;

	std::shared_ptr<Wt::WStandardItemModel> buildHistoricDataModel();

};

