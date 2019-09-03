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
#if LAUNCH_WEB_SERVER
#include <Wt/WSignal.h> // Signals support for the web server
#endif // LAUNCH_WEB_SERVER
#include "External/CSVWriter.h"
#include "StateMonitor.h"
#include "OutputState.h"

typedef std::chrono::system_clock Clock;

//// Scheduler
#include "External/Scheduler/Scheduler.h"

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
	CSVWriter csv;
	string filename = "outputFile.csv";
	string outputDirectory = "output_data/"; // should end in a slash if it's not empty
	string fileFullPath = "output_data/outputFile.csv";

	// Variables for holding the last read values
	StateMonitor* monitor;
	char * inputPortNames[NUM_CHANNELS] = globalLabjackInputPortNames;
	char * inputPortPurpose[NUM_CHANNELS] = globalLabjackInputPortPurpose;
	double previousReadInputPortValues[NUM_CHANNELS] = {0.0};
	double lastReadInputPortValues[NUM_CHANNELS] = {0.0};
	bool inputPortValuesChanged[NUM_CHANNELS] = {false};
	int errorAddress;

	// Vector of Output Port Objects
	char* outputPortNames[NUM_OUTPUT_CHANNELS] = globalLabjackOutputPortNames;
	std::vector<OutputState*> outputPorts = {};

	// Time Keeping
	std::chrono::time_point<Clock> lastCaptureComputerTime;
	Bosma::Scheduler* scheduler; // For wallTime based scheduling

	// The time each water port LED should remain lit after a dispense event
	std::chrono::time_point<Clock> water1PortEndIlluminationTime;
	std::chrono::time_point<Clock> water2PortEndIlluminationTime;


	// Scheduled tasks
	void runTopOfHourUpdate(); // Runs at the top of every hour (exactly on the hour, according to system time).
	//void runTopOfMinuteUpdate();

	// Mutex/Synchronization:
	std::mutex logMutex;


	// Visible Light Relay Control
	//void setVisibleLightRelayState(bool isOn);

#if LAUNCH_WEB_SERVER
	// Signals
	// <int: serialNumber, int: portIndex, double: newValue>
	Wt::Signal<int, int, double> valueChanged_;
#endif // LAUNCH_WEB_SERVER

};


