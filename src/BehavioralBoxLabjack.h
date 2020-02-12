/**
 * Name: BehavioralBoxLabjack.h
 * Desc: An object representing a single labjack used inside a behavioral box
 // Want to read in sensor values in Labjack's "Command-response" mode for minimum latency
**/
#pragma once
#include "config.h"
#include <time.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <regex>
#if LAUNCH_WEB_SERVER
#include <Wt/WSignal.h> // Signals support for the web server
#endif // LAUNCH_WEB_SERVER
#include "External/CSVWriter.h"
#include "StateMonitor.h"
#include "OutputState.h"
#include "ConfigurationManager.h"


typedef std::chrono::system_clock Clock;

//// Scheduler
#include "External/Scheduler/Scheduler.h"

// REGEX:
static const std::regex behavioral_box_labjack_deviceName_regex("LJ-(\\d{2})");

class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, int serialNumber);
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, int serialNumber);
	BehavioralBoxLabjack(const BehavioralBoxLabjack&) = delete; // non construction-copyable
	BehavioralBoxLabjack& operator=(const BehavioralBoxLabjack&) = delete; // non copyable
	~BehavioralBoxLabjack();

	void diagnosticPrint();
	void printIdentifierLine();
	void diagnosticPrintLastValues();
	int getError();

	// Time
	time_t getTime();
	void setTime(time_t newTime);

	// Syncs the Labjack's internal RTC time with the computer's. Returns the number of seconds that were adjusted to set the Labjack's clock.
	double syncDeviceTimes();

	// Light Control
	bool isArtificialDaylightHours();
	bool isAttractModeLEDLit(int portNumber);
	void updateVisibleLightRelayIfNeeded();

	// Write Output Pin values
	void writeOutputPinValues();
	void writeOutputPinValues(bool shouldForceWrite);
	

	// Read Sensor values
	void readSensorValues();
	void persistReadValues(bool enableConsoleLogging);
	//void persistReadValues();

	// Main run loop
	void runPollingLoop();

	// Getters:
	int getSerialNumber() { return this->serialNumber; }
	std::string getDeviceName() { return this->deviceName; }
	bool isVisibleLEDLit();
	string getFullFilePath() { return this->fileFullPath; }
	int getNumberInputChannels() { return NUM_CHANNELS; }
	int getNumberOutputChannels() { return NUM_OUTPUT_CHANNELS; }
	vector<std::string> getInputPortNames();
	vector<std::string> getInputPortPurpose();
	vector<double> getLastReadValues();
	string getOutputDirectory() { return this->outputDirectory; }

#if LAUNCH_WEB_SERVER
	Wt::Signal<int, int, double>& valueChanged() { return this->valueChanged_; }
#endif // LAUNCH_WEB_SERVER

	// Override Functions
	void toggleOverrideMode_VisibleLED();
	void toggleOverrideMode_AttractModeLEDs();

private:
	int serialNumber;
	int uniqueIdentifier;
	std::string deviceName = "";
	int portOrPipe, ipAddress, packetMaxBytes;
	int deviceType;
	int connectionType;
	int err;
	int handle;
	bool shouldStop = false;

	// Override Values: 
	bool isOverrideActive_VisibleLED = false;
	bool overrideValue_isVisibleLEDLit = false;

	bool isOverrideActive_AttractModeLEDs = false;
	bool overrideValue_areAttractModeLEDsLit = false;

	// File Output:
	std::shared_ptr<ConfigurationManager> configMan = make_shared<ConfigurationManager>();
	CSVWriter csv;
	string filename = "outputFile.csv";
	string outputDirectory = configMan->getGeneratedActiveOutputDirectory(); // should end in a slash if it's not empty
	string fileFullPath = "C:/Common/data/outputFile.csv";

	CSVWriter csv_analog;
	string filename_analog = "outputFile_analog.csv";
	string fileFullPath_analog = "C:/Common/data/outputFile_analog.csv";


	// Variables for holding the last read values
	StateMonitor* monitor;
	// Digital values:
	char * inputPortNames[NUM_CHANNELS] = globalLabjackInputPortNames;
	char * inputPortPurpose[NUM_CHANNELS] = globalLabjackInputPortPurpose;
	double previousReadInputPortValues[NUM_CHANNELS] = {0.0};
	double lastReadInputPortValues[NUM_CHANNELS] = {0.0};
	bool inputPortValuesChanged[NUM_CHANNELS] = {false};

	// Analog Values:
	//TODO: Check if we need a separate StateMonitor
	//TODO: Check how CSVWriter is initialized
	char* inputPortNames_analog[NUM_CHANNELS_ANALOG] = globalLabjackAnalogInputPortNames;
	char* inputPortPurpose_analog[NUM_CHANNELS_ANALOG] = globalLabjackAnalogInputPortPurpose;
	double previousReadInputPortValues_analog[NUM_CHANNELS_ANALOG] = { 0.0 };
	double lastReadInputPortValues_analog[NUM_CHANNELS_ANALOG] = { 0.0 };
	bool inputPortValuesChanged_analog[NUM_CHANNELS_ANALOG] = { false };


	int errorAddress;

	// Vector of Output Port Objects
	char* outputPortNames[NUM_OUTPUT_CHANNELS] = globalLabjackOutputPortNames;
	std::vector<OutputState*> outputPorts = {};

	std::string readDeviceName();
	bool writeDeviceName(std::string newDeviceName);

	void initializeLabjackConfigurationIfNeeded(); // Configures Labjack's name and input/output port mappings if it hasn't already been done.

	// Time Keeping
	std::chrono::time_point<Clock> lastCaptureComputerTime;
	Bosma::Scheduler* scheduler; // For wallTime based scheduling

	// The time each water port LED should remain lit after a dispense event
	std::chrono::time_point<Clock> water1PortEndIlluminationTime;
	std::chrono::time_point<Clock> water2PortEndIlluminationTime;


	// Scheduled tasks


	// Mutex/Synchronization:
	std::mutex logMutex;
	std::mutex labjackMutex;

	// Visible Light Relay Control
	//void setVisibleLightRelayState(bool isOn);

#if LAUNCH_WEB_SERVER
	// Signals
	// <int: serialNumber, int: portIndex, double: newValue>
	Wt::Signal<int, int, double> valueChanged_;
#endif // LAUNCH_WEB_SERVER

};


