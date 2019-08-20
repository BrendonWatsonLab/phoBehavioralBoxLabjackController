
#include "LabjackExample.h"

#include <Wt/WApplication.h>
#include <Wt/WDate.h>
#include <Wt/WEnvironment.h>
#include <Wt/WItemDelegate.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WText.h>

#include <Wt/WBorderLayout.h>
#include <Wt/WFitLayout.h>

#include <Wt/WStandardItem.h>
#include <Wt/WTableView.h>

using namespace Wt;

LabjackExample::LabjackExample(): WContainerWidget()
{
	this->activeLabjacks = std::vector<BehavioralBoxLabjack*>();
	//wt_root = (PageRoot);
	//wt_root->setId("wt_root");
	//wt_root->setStyleClass(Wt::WString::fromUTF8(""));
	//wt_root->setInline(0);
	//this->addWidget(cpp14::make_unique<WGroupBox>(WString::tr("Labjack Example")));
	this->groupbox = this->addWidget(cpp14::make_unique<WGroupBox>());
	//groupbox = new WGroupBox(wt_root);
	/*this->groupbox->setId("groupbox");
	this->groupbox->setStyleClass(Wt::WString::fromUTF8(""));
	this->groupbox->setInline(1);
	this->groupbox->setTitle(Wt::WString::fromUTF8("Some Title"));*/
	//{
		//text_cp = new Wt::WText(groupbox);
		//text_cp->setId("text_cp");
		//text_cp->setStyleClass(Wt::WString::fromUTF8(""));
		//text_cp->setInline(0);
		//text_cp->setTextFormat((Wt::TextFormat)0);
		//text_cp->setText(Wt::WString::fromUTF8("Some Text"));
		//text_cp_cp = new Wt::WText(groupbox);
		//text_cp_cp->setId("text_cp_cp");
		//text_cp_cp->setStyleClass(Wt::WString::fromUTF8(""));
		//text_cp_cp->setInline(0);
		//text_cp_cp->setTextFormat((Wt::TextFormat)0);
		//text_cp_cp->setText(Wt::WString::fromUTF8("Some Text"));
	//}
	this->text_labjack_label = this->addWidget(cpp14::make_unique<WText>(WString::tr("Labjack Example")));
	this->text_number_of_labjacks = this->addWidget(cpp14::make_unique<WText>(WString::tr("Finding Labjacks...")));

	//btnRefresh = new Wt::WPushButton(this->groupbox);
	btnRefresh = this->addWidget(cpp14::make_unique<Wt::WPushButton>(tr("save")));
	btnRefresh->setId("btnRefresh");
	btnRefresh->setStyleClass(Wt::WString::fromUTF8("btn with-label btn-default btn btn-default with-label undefined"));
	btnRefresh->setInline(1);
	btnRefresh->setText(Wt::WString::fromUTF8("Refresh"));
	//btnRefresh->bindSafe()
	btnRefresh->clicked().connect(this, &LabjackExample::tryFetchNewLabjacks);

	this->setupInterface();
}

LabjackExample::LabjackExample(std::vector<BehavioralBoxLabjack*> updatedLabjacks): LabjackExample()
{
	this->activeLabjacks = updatedLabjacks;
	this->refreshInterface();
}

void LabjackExample::updateLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks)
{
	this->activeLabjacks = updatedLabjacks;
	this->refreshInterface();
}

void LabjackExample::tryFetchNewLabjacks()
{
	//this->text_number_of_labjacks->setText("CHANGING IT!");
	this->text_number_of_labjacks->setText(WString::tr("CHANGING IT!"));
	this->text_number_of_labjacks->refresh();
}

void LabjackExample::setupInterface()
{
	this->text_labjack_label->setId("text_number_of_labjacks");
	this->text_labjack_label->setInline(false);
	this->text_labjack_label->setTextFormat((Wt::TextFormat)0);

	this->text_number_of_labjacks->setId("text_number_of_labjacks");
	this->text_number_of_labjacks->setInline(false);
	this->text_number_of_labjacks->setTextFormat((Wt::TextFormat)0);
}

void LabjackExample::refreshInterface()
{
	if (this->activeLabjacks.size() > 0) {
		/*WString newString = to_wstring(this->activeLabjacks.size()) + WString::tr(" Labjacks");*/
		std::string newString = std::to_string(this->activeLabjacks.size()) + " Labjacks";
		this->text_number_of_labjacks->setText(newString);
		//this->addWidget(cpp14::make_unique<WText>(to_wstring(this->activeLabjacks.size())));
	}
	else {
		//this->text_number_of_labjacks->setText(WString::tr("0 Labjacks"));
		this->addWidget(cpp14::make_unique<WText>("No Labjacks"));
	}
	this->text_number_of_labjacks->show();
	this->text_number_of_labjacks->refresh();
}
