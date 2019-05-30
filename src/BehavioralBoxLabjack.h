/**
 * Name: BehavioralBoxLabjack.h
 * Desc: An object representing a single labjack used inside a behavioral box
 // Want to read in sensor values in Labjack's "Command-response" mode for minimum latency
**/

#include <time.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include "External/CSVWriter.h"

typedef std::chrono::system_clock Clock;

#pragma once
class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, const char * iden);
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden);
	~BehavioralBoxLabjack();

	void diagnosticPrint();
	int getError();

	// Time
	time_t getTime();
	void setTime(time_t newTime);

	// Syncs the Labjack's internal RTC time with the computer's. Returns the number of seconds that were adjusted to set the Labjack's clock.
	double syncDeviceTimes();


	// Visible Light Relay Control
	void setVisibleLightRelayState(bool isOn);

	// Read Sensor values
	void readSensorValues();
	void persistReadValues();




private:
	int uniqueIdentifier;
	int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
	int deviceType;
	int connectionType;
	int err;
	int handle;

	// File Output:
	CSVWriter csv;
	string filename = "outputFile.csv";
	string outputDirectory = "output_data/"; // should end in a slash if it's not empty
	string fileFullPath = "output_data/outputFile.csv";

	// Variables for holding the last read values
	char * inputPortNames[9] = {"DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0"};
	double previousReadInputPortValues[9] = {0,0,0,0,0,0,0,0,0};
	double lastReadInputPortValues[9] = { 0,0,0,0,0,0,0,0,0};
	bool inputPortValuesChanged[9] = {false, false, false, false, false, false, false, false, false};
	int errorAddress;
	//time_t lastCaptureComputerTime;
	std::chrono::time_point<Clock> lastCaptureComputerTime;

	
};

