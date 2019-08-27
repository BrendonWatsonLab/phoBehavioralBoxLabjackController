#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

class TimeSeriesChart : public Wt::WContainerWidget
{
public:
	TimeSeriesChart();

	std::shared_ptr<Wt::WAbstractItemModel> buildHistoricDataModel();

};

