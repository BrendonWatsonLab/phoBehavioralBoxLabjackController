#include <limits>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "BehavioralBoxLajbackController.h"

BehavioralBoxLajbackController::BehavioralBoxLajbackController()
{
	this->thread_ = std::thread(std::bind(&BehavioralBoxLajbackController::run, this));
}

BehavioralBoxLajbackController::~BehavioralBoxLajbackController()
{
	this->shouldStop_ = true;
	this->thread_.join();
}

void BehavioralBoxLajbackController::connect(Client* client, const std::function<void()>& function)
{
	std::unique_lock<std::mutex> lock(mutex_);
	connections_.push_back(Connection(Wt::WApplication::instance()->sessionId(), client, function));
}

void BehavioralBoxLajbackController::disconnect(Client* client)
{
	std::unique_lock<std::mutex> lock(mutex_);

	for (unsigned i = 0; i < connections_.size(); ++i) {
		if (connections_[i].client == client) {
			connections_.erase(connections_.begin() + i);
			return;
		}
	}
	assert(false);
}

void BehavioralBoxLajbackController::run()
{
	/*
   * This method simulates changes to the data that happen in a background
   * thread.
   */
	for (;;) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (this->shouldStop_) {
			return;
		}

		{
			std::unique_lock<std::mutex> lock(mutex_);
			//++counter_;
			//TODO: Update the number of labjacks and such
			// **HERE**


			// **HERE**
			/* This is where we notify all connected clients. */
			for (unsigned i = 0; i < connections_.size(); ++i) {
				Connection& c = connections_[i];
				Wt::WServer::instance()->post(c.sessionId, c.function);
			}
		}
	}
}

void BehavioralBoxLajbackController::loadHistoricalData()
{
	/*for (int i = 0; i < this->getActiveLabjacks().size(); i++) {
		std::vector<LabjackDataFile> currLabjackDataFile = this->findDataFiles(this->getActiveLabjacks()[i]->getOutputDirectory(), this->getActiveLabjacks()[i]->getSerialNumber());
		
	}*/
}

//// NOTE: the start/end milliseconds are non-inclusive
//std::vector<LabjackDataFile> BehavioralBoxLajbackController::findDataFiles(std::string searchDirectory, int labjackSerialNumber, unsigned long long startMillisecondsSinceEpoch, unsigned long long endMillisecondsSinceEpoch)
//{
//	// Looks for files of the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}.csv"
//	// Build Serial Number String:
//	string expectedSerialNumberString = "s" + std::to_string(labjackSerialNumber);
//	std::vector<LabjackDataFile> outputVector = std::vector<LabjackDataFile>();
//	//fs::path dir = "output_data/";
//	fs::path dir = searchDirectory;
//	std::vector<fs::path> foundFiles = scan_csv_files(dir);
//	// Vector of string to save tokens 
//	vector <string> fileNameParts;
//	string intermediate;
//	// Loop through the found files
//	for (int i = 0; i < foundFiles.size(); i++) {
//		std::string currFileBasename = foundFiles[i].stem().string(); // Stem returns the filename without the extension.
//		// stringstream class check1 
//		stringstream currNameSplitterStream(currFileBasename);
//		// Tokenizing w.r.t. underscore '_' 
//		while (getline(currNameSplitterStream, intermediate, '_'))
//		{
//			fileNameParts.push_back(intermediate);
//		}
//		if (fileNameParts.size() != 4) {
//			cout << "Warning: File " << currFileBasename << " does not match the expected format (out_file_s{ SERIAL_NUMBER }_{ MILLISECONDS_SINCE_EPOCH }). Skipping." << endl;
//			continue;
//		}
//		// Check if the serial number matches the search serial number
//		if (fileNameParts[2] != expectedSerialNumberString) {
//			// Serial number doesn't match
//			continue;
//		}
//		// Parse the milliseconds component
//		string millisecondsComponentString = fileNameParts[3];
//		unsigned long long millisecondsComponent = 0;
//		try {
//			millisecondsComponent = std::stoull(millisecondsComponentString, NULL, 0);
//		}
//		catch (...) {
//			// Couldn't parse it to a unsigned long long
//			cout << "Warning: File " << currFileBasename << " could not parse the milliseconds component " << millisecondsComponentString << " to an unsigned long long. Skipping." << endl;
//			continue;
//		}
//		// Check whether it's within the time range the user provided
//		if ((millisecondsComponent < startMillisecondsSinceEpoch) || (millisecondsComponent > endMillisecondsSinceEpoch)) {
//			// It's outisde the available range
//			continue;
//		}
//		// It meets all criteria to be returned. Add it to the output vector
//		LabjackDataFile currFile = LabjackDataFile(foundFiles[i], labjackSerialNumber, millisecondsComponent);
//		outputVector.push_back(currFile);
//		// Clear the name parts for the next file
//		fileNameParts.clear();
//	}
//	return outputVector;
//}
//
//std::vector<LabjackDataFile> BehavioralBoxLajbackController::findDataFiles(std::string searchDirectory, int labjackSerialNumber)
//{
//	return findDataFiles(searchDirectory, labjackSerialNumber, 0, std::numeric_limits<unsigned long long>::max());
//}
