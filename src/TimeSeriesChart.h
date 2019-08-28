#pragma once
#include <vector>
#include <string>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WAbstractItemModel.h>

class TimeSeriesChart : public Wt::WContainerWidget
{
public:
	TimeSeriesChart();

	bool shouldUseDateXAxis = true;

	std::shared_ptr<Wt::WStandardItemModel> buildHistoricDataModel();

	std::vector<Wt::WColor> getVariableColors() { return this->colorVect_; }
	Wt::WColor getDefaultColor() { return this->otherColor_; }

private:
	void setupTable(const std::shared_ptr<Wt::WAbstractItemModel> model);
	void setupCharts(const std::shared_ptr<Wt::WAbstractItemModel> model);
	std::vector<std::unique_ptr<Wt::Chart::WDataSeries>> buildDataSeries(const std::shared_ptr<Wt::WAbstractItemModel> model);

	std::vector<Wt::WColor> colorVect_ = { Wt::WColor(0, 255, 255, 255), Wt::WColor(127, 255, 212, 255), Wt::WColor(255, 127, 80, 255), Wt::WColor(255, 0, 255, 255), Wt::WColor(0, 0, 255, 255), Wt::WColor(0, 0, 139, 255), Wt::WColor(220, 20, 60, 255), Wt::WColor(128, 0, 0, 255) };
	Wt::WColor otherColor_ = Wt::WColor(200, 200, 200, 255);
};

