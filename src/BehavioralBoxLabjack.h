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
#include "External/CSVWriter.h"

typedef std::chrono::system_clock Clock;

//// Scheduler
#include "External/Scheduler/Scheduler.h"

class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, const char * iden);
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden);
	BehavioralBoxLabjack(const BehavioralBoxLabjack&) = delete; // non construction-copyable
	BehavioralBoxLabjack& operator=(const BehavioralBoxLabjack&) = delete; // non copyable
	~BehavioralBoxLabjack();

	void diagnosticPrint();
	int getError();

	// Time
	time_t getTime();
	void setTime(time_t newTime);

	// Syncs the Labjack's internal RTC time with the computer's. Returns the number of seconds that were adjusted to set the Labjack's clock.
	double syncDeviceTimes();

	// Light Control
	bool isArtificialDaylightHours();
	void updateVisibleLightRelayIfNeeded();

	// Read Sensor values
	void readSensorValues();
	void persistReadValues();

	// Main run loop
	void runPollingLoop();




private:
	int uniqueIdentifier;
	int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
	int deviceType;
	int connectionType;
	int err;
	int handle;
	bool shouldStop = false;

	// File Output:
	CSVWriter csv;
	string filename = "outputFile.csv";
	string outputDirectory = "output_data/"; // should end in a slash if it's not empty
	string fileFullPath = "output_data/outputFile.csv";

	// Variables for holding the last read values
	char * inputPortNames[NUM_CHANNELS] = globalLabjackInputPortNames;
	double previousReadInputPortValues[NUM_CHANNELS] = {0.0};
	double lastReadInputPortValues[NUM_CHANNELS] = {0.0};
	bool inputPortValuesChanged[NUM_CHANNELS] = {false};
	int errorAddress;

	// Time Keeping
	std::chrono::time_point<Clock> lastCaptureComputerTime;
	Bosma::Scheduler* scheduler; // For wallTime based scheduling

	// Scheduled tasks
	void runTopOfHourUpdate(); // Runs at the top of every hour (exactly on the hour, according to system time).
	//void runTopOfMinuteUpdate();

	// Visible Light Relay Control
	void setVisibleLightRelayState(bool isOn);

};

