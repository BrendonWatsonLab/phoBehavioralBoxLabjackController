
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

#include "NumericItem.h"

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
	this->text_labjack_label = this->groupbox->addWidget(cpp14::make_unique<WText>(WString("Found Labjacks:")));
	this->text_number_of_labjacks = this->groupbox->addWidget(cpp14::make_unique<WText>(WString("Finding Labjacks...")));

	//btnRefresh = new Wt::WPushButton(this->groupbox);
	btnRefresh = this->groupbox->addWidget(cpp14::make_unique<Wt::WPushButton>(tr("save")));
	btnRefresh->setId("btnRefresh");
	btnRefresh->setStyleClass(Wt::WString::fromUTF8("btn with-label btn-default btn btn-default with-label undefined"));
	btnRefresh->setInline(1);
	btnRefresh->setText(Wt::WString::fromUTF8("Refresh"));
	//btnRefresh->bindSafe()
	btnRefresh->clicked().connect(this, &LabjackExample::tryFetchNewLabjacks);

	// Add the Table
	// Show a view that allows editing of the model.
	this->liveContainer = this->addWidget(cpp14::make_unique<WContainerWidget>());
	this->tblLiveLabjackData = this->liveContainer->addWidget(cpp14::make_unique<WTableView>());

	this->onActiveLabjacksChanged();
	this->setupInterface();
}

LabjackExample::LabjackExample(std::vector<BehavioralBoxLabjack*> updatedLabjacks): LabjackExample()
{
	this->activeLabjacks = updatedLabjacks;
	this->onActiveLabjacksChanged();
	this->refreshInterface();
}

void LabjackExample::updateLabjacks(std::vector<BehavioralBoxLabjack*> updatedLabjacks)
{
	this->activeLabjacks = updatedLabjacks;
	this->onActiveLabjacksChanged();
	this->refreshInterface();
}

void LabjackExample::tryFetchNewLabjacks()
{
	this->refreshInterface();
}

// Loop through the new labjacks to set them up
void LabjackExample::onActiveLabjacksChanged()
{
	this->mapLabjackSerialNumberToRow.clear();
	for (int i = 0; i < this->activeLabjacks.size(); i++) {
		// Add the labjack's serialNumber to the map
		this->mapLabjackSerialNumberToRow.insert(std::make_pair(i, this->activeLabjacks[i]->getSerialNumber()));
		// Bind the "onLabjackValueChanged" function to the labjack's "valueChanged()" signal
		//this->activeLabjacks[i]->valueChanged().connect(this, &LabjackExample::onLabjackValueChanged);
	}
}

void LabjackExample::setupInterface()
{
	this->text_labjack_label->setId("text_labjack_label");
	this->text_labjack_label->setInline(false);
	this->text_labjack_label->setTextFormat((Wt::TextFormat)0);

	this->text_number_of_labjacks->setId("text_number_of_labjacks");
	this->text_number_of_labjacks->setInline(false);
	this->text_number_of_labjacks->setTextFormat((Wt::TextFormat)0);

	// Live Labjack State Table:
	this->tblLiveLabjackData->setMargin(10, Side::Top | Side::Bottom);
  this->tblLiveLabjackData->setMargin(WLength::Auto, Side::Left | Side::Right);
  
  this->tblLiveLabjackData->setSortingEnabled(false);
  this->tblLiveLabjackData->setColumnResizeEnabled(false);
  // this->tblLiveLabjackData->setSelectionMode(SelectionMode::Extended);
  this->tblLiveLabjackData->setAlternatingRowColors(true);
  this->tblLiveLabjackData->setColumnAlignment(0, AlignmentFlag::Center);
  this->tblLiveLabjackData->setHeaderAlignment(0, AlignmentFlag::Center);
  this->tblLiveLabjackData->setRowHeight(22);

  // Disable Editing of the table:
  this->tblLiveLabjackData->resize(1400, WLength::Auto);
  //this->tblLiveLabjackData->setEditTriggers(EditTrigger::None);
  this->tblLiveLabjackData->setEditTriggers(EditTrigger::SingleClicked);

  // We use a single delegate for all items which rounds values to
// the closest integer value.
  std::shared_ptr<WItemDelegate> tableEditDelegate = std::make_shared<WItemDelegate>();
  tableEditDelegate->setTextFormat("%.f");
  this->tblLiveLabjackData->setItemDelegate(tableEditDelegate);

  this->tblLiveLabjackData->setColumnWidth(0, 140);
  //this->updateTableModel();
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
		this->text_number_of_labjacks->setText("No Labjacks");
		this->text_number_of_labjacks->refresh();
		//this->addWidget(cpp14::make_unique<WText>("No Labjacks"));
	}
	this->text_number_of_labjacks->show();
	this->text_number_of_labjacks->refresh();

	// Update table:
	this->updateTableModel();
	this->tblLiveLabjackData->resize(1400, WLength::Auto);
	this->tblLiveLabjackData->setColumnWidth(0, 140);
	for (int i = 1; i < this->liveLabjackTableModel->columnCount(); ++i) {
		this->tblLiveLabjackData->setColumnWidth(i, 140);
	}
}

void LabjackExample::updateTableModel()
{
	// Read the CSV file
	this->liveLabjackTableModel = this->buildLiveLabjacksModel();
	this->tblLiveLabjackData->setModel(this->liveLabjackTableModel);
}

std::shared_ptr<WAbstractItemModel> LabjackExample::buildLiveLabjacksModel()
{
	std::shared_ptr<WStandardItemModel> model = std::make_shared<WStandardItemModel>(0, 0);
	std::unique_ptr<NumericItem> prototype = cpp14::make_unique<NumericItem>();
	model->setItemPrototype(std::move(prototype));

	// Iterate through labjacks (rows)
	for (int rowIndex = 0; rowIndex < this->activeLabjacks.size(); rowIndex++) {
		// Loop through columns:
		int currColumnNumber = 0;

		BehavioralBoxLabjack* currLabjack = this->activeLabjacks[rowIndex];
		vector<std::string> portPurposeStrings = currLabjack->getInputPortPurpose();
		vector<double> mostRecentValues = currLabjack->getLastReadValues();

		// Want a column for the serial number
		int currSerialNumber = currLabjack->getSerialNumber();
		int currNumInputChannels = currLabjack->getNumberInputChannels();

		int currTotalTableColumns = currNumInputChannels + 1;

		for (int columnIndex = 0; columnIndex < currTotalTableColumns; columnIndex++) {
			//currColumnNumber: the 1-indexed column identifier
			currColumnNumber = columnIndex + 1;
			// Add extra columns to model if needed
			if (currColumnNumber >= model->columnCount())
				model->insertColumns(model->columnCount(),
					currColumnNumber + 1 - model->columnCount());

			// If it's the first row (we only set the header data once)
			if (rowIndex == 0) {
				if (columnIndex == 0) {
					// It's the serial number column
					std::string serialNumberString = std::to_string(currSerialNumber);
					model->setHeaderData(columnIndex, Wt::cpp17::any(Wt::WString("SerialNum")));
				}
				else {
					int dataColumnIndex = columnIndex - 1; // Subtract off the first (serialNumber) index
					// It's a data column
					model->setHeaderData(columnIndex, Wt::cpp17::any(Wt::WString(portPurposeStrings[dataColumnIndex])));
				}
				
			}
			// Add the row if needed:
			if (rowIndex >= model->rowCount()) {
				model->insertRows(model->rowCount(), rowIndex + 1 - model->rowCount());
			}
			// Finally, add the data
			Wt::cpp17::any data;

			if (columnIndex == 0) {
				// It's the serial number column
				std::string serialNumberString = std::to_string(currSerialNumber);
				data = Wt::cpp17::any(Wt::WString(serialNumberString));
			}
			else {
				// It's a data column
				int dataColumnIndex = columnIndex - 1; // Subtract off the first (serialNumber) index
				double currValue = mostRecentValues[dataColumnIndex];
				data = Wt::cpp17::any(currValue);
			}

			//TODO: do I used 0 or 1-indexed column
			model->setData(rowIndex, columnIndex, data);
			model->item(rowIndex, columnIndex)->setFlags(ItemFlag::Selectable | ItemFlag::Editable);
		} // end column loop

	} // end row (Labjack) loop

	//for (int row = 0; row < model->rowCount(); ++row) {
	//	for (int col = 0; col < model->columnCount(); ++col) {
	//		//model->item(row, col)->setFlags(ItemFlag::Selectable);
	//		model->item(row, col)->setFlags(ItemFlag::Selectable | ItemFlag::Editable);
	//		

	//		/*
	//		  Example of tool tips (disabled here because they are not updated
	//		  when editing data)
	//		 */

	//		 /*
	//		 WString toolTip = asString(model->headerData(col)) + ": "
	//		   + asString(model->item(row, col)->data(DisplayRole), "%.f");
	//		 model->item(row, col)->setToolTip(toolTip);
	//		  */
	//	}
	//}
	return model;
}

// Called when a value for one of the child labjacks changes
void LabjackExample::onLabjackValueChanged(int labjackSerialNumber, int portIndex, double newValue)
{
	if (labjackSerialNumber <= 0) {
		//Ignore erronious labjacks
		return;
	}

	//// Get row index
	//int tableRowIndex = this->mapLabjackSerialNumberToRow[labjackSerialNumber];
	//int tableColumnIndex = portIndex + 1; // Add one to account for the serialNumber column
	//// Finally, add the data
	//std::string newValueString = std::to_string(newValue);
	//Wt::cpp17::any updatedData;
	//
	//updatedData = Wt::cpp17::any(newValue);

	this->updateTableModel();
	//updatedData = Wt::cpp17::any(Wt::WString(newValueString));
	//updatedData = Wt::cpp17::any(newValueString);
	// Update the table:
	//this->liveLabjackTableModel->setData(tableRowIndex, tableColumnIndex, updatedData);
}
