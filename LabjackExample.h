#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WGroupBox.h>
#include <Wt/WText.h>
#include "src/BehavioralBoxLabjack.h"
/**
 * \defgroup LabjackExample Labjack Web example
 */
 /*@{*/

 /*! \brief A widget that demonstrates a labjack status
  */
class LabjackExample: public Wt::WContainerWidget
{
public:
	/*! \brief Creates the labjack web example
	 */
	LabjackExample();
	LabjackExample(std::vector<BehavioralBoxLabjack*> updatedLabjacks);

	void updateLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks);

	//Wt::WContainerWidget* wt_root;
	Wt::WGroupBox* groupbox;
	Wt::WText* text_labjack_label;
	Wt::WText* text_number_of_labjacks;

private:
	std::vector<BehavioralBoxLabjack*> activeLabjacks;
	void setupInterface();
	void refreshInterface();
};

