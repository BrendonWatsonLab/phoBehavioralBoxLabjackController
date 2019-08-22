#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "BehavioralBoxLabjack.h"
/**
 * \defgroup LabjackLiveStateWidget Labjack Web example
 */
 /*@{*/

 /*! \brief A widget that demonstrates a labjack status
  */
class LabjackLiveStateWidget: public Wt::WContainerWidget
{
public:
	/*! \brief Creates the labjack web example
	 */
	LabjackLiveStateWidget();
	LabjackLiveStateWidget(std::vector<BehavioralBoxLabjack*> updatedLabjacks);

	void updateLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);

	//Wt::WContainerWidget* wt_root;
	Wt::WGroupBox* groupbox;
	Wt::WText* text_labjack_label;
	Wt::WText* text_number_of_labjacks;

	Wt::WPushButton* btnRefresh;

	Wt::WContainerWidget* liveContainer;
	Wt::WTableView* tblLiveLabjackData;

private:
	std::vector<BehavioralBoxLabjack*> activeLabjacks;
	std::map<int, int> mapLabjackSerialNumberToRow;

	void tryFetchNewLabjacks();
	void onActiveLabjacksChanged();
	void setupInterface();
	void refreshInterface();

	// Table
	std::shared_ptr<Wt::WAbstractItemModel> liveLabjackTableModel;
	void updateTableModel();

	std::shared_ptr<Wt::WAbstractItemModel> buildLiveLabjacksModel();
	// Signal/Event Handling:
	void onLabjackValueChanged(int labjackSerialNumber, int portIndex, double newValue);

};

