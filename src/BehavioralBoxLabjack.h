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


#include "LabjackStreamHelpers.h"


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
	// void readSensorValues();
	

	void persistReadValues(bool enableConsoleLogging);
	//void persistReadValues();

	// Main run loop
	void runPollingLoop();

	// Getters:
	int getSerialNumber() { return this->serialNumber; }
	std::string getDeviceName() { return this->deviceName; }
	bool isVisibleLEDLit();
	std::string getFullFilePath() { return this->fileFullPath; }
	int getNumberInputChannels(bool include_digital_ports = true, bool include_analog_ports = false);
	int getNumberOutputChannels() { return NUM_OUTPUT_CHANNELS; }
	std::vector<std::string> getInputPortNames(bool include_digital_ports = true, bool include_analog_ports = false);
	std::vector<std::string> getInputPortPurpose(bool include_digital_ports = true, bool include_analog_ports = false);
	std::vector<double> getLastReadValues(bool include_digital_ports = true, bool include_analog_ports = false);
	std::string getOutputDirectory() { return this->outputDirectory; }

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
	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();
	CSVWriter csv;
	std::string filename = "outputFile.csv";
	std::string outputDirectory = configMan->getGeneratedActiveOutputDirectory(); // should end in a slash if it's not empty
	std::string fileFullPath = "C:/Common/data/outputFile.csv";

	CSVWriter csv_analog;
	std::string filename_analog = "outputFile_analog.csv";
	std::string fileFullPath_analog = "C:/Common/data/outputFile_analog.csv";


	// Labjack Stream-specific values:
	StreamInfo ljStreamInfo;
	// Variables for holding the last read values
	StateMonitor* monitor;
	// All Values:
	char * inputPortNames_all[NUM_CHANNELS] = globalLabjackInputPortNames;
	char * inputPortPurpose_all[NUM_CHANNELS] = globalLabjackInputPortPurpose;
	bool inputPortIsAnalog[NUM_CHANNELS] = globalLabjackInputPortIsAnalog;
	double previousReadInputPortValues_all[NUM_CHANNELS] = {0.0};
	double lastReadInputPortValues[NUM_CHANNELS] = {0.0};
	bool inputPortValuesChanged[NUM_CHANNELS] = {false};

	// Digital values:
	char* inputPortNames_digital[NUM_CHANNELS_DIGITAL] = globalLabjackDigitalInputPortNames;
	char* inputPortPurpose_digital[NUM_CHANNELS_DIGITAL] = globalLabjackDigitalInputPortPurpose;

	// Analog Values:
	//TODO: Check if we need a separate StateMonitor
	//TODO: Check how CSVWriter is initialized
	char* inputPortNames_analog[NUM_CHANNELS_ANALOG] = globalLabjackAnalogInputPortNames;
	char* inputPortPurpose_analog[NUM_CHANNELS_ANALOG] = globalLabjackAnalogInputPortPurpose;

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


