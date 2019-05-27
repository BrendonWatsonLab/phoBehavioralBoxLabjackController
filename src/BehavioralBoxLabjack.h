/**
 * Name: BehavioralBoxLabjack.h
 * Desc: An object representing a single labjack used inside a behavioral box
 // Want to read in sensor values in Labjack's "Command-response" mode for minimum latency
**/

#include <LabJackM.h>
#include <time.h>
#include <iostream>
#include <fstream>


#pragma once
class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden, std::ofstream& outFile);
	~BehavioralBoxLabjack();

	void diagnosticPrint();
	int getError();

	// Time
	time_t getTime();
	void setTime(time_t newTime);

	// Visible Light Relay Control
	void setVisibleLightRelayState(bool isOn);

	// Read Sensor values
	void readSensorValues();
	void persistReadValues();


private:
	int uniqueIdentifier;
	int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
	int deviceType = LJM_dtANY;
	int connectionType = LJM_ctANY;
	int err;
	int handle;

	// File Output:
	std::ofstream& outputFile;

	char string[LJM_STRING_ALLOCATION_SIZE];

	// Variables for holding the last read values
	char * inputPortNames[9] = {"DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0"};
	double previousReadInputPortValues[9] = {0,0,0,0,0,0,0,0,0};
	double lastReadInputPortValues[9] = { 0,0,0,0,0,0,0,0,0};
	bool inputPortValuesChanged[9] = {false, false, false, false, false, false, false, false, false};
	int errorAddress;
	time_t lastCaptureComputerTime;
	
};

