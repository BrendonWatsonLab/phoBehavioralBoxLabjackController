#include "BehavioralBoxLabjack.h"
/**
 * Name: BehavioralBoxLabjack.cpp
 * Desc: An object representing a single labjack used inside a behavioral box
**/

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>  // For inet_ntoa()
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <LabJackM.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"

// Constructor: Called when an instance of the object is about to be created
BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden, std::ofstream& outFile): deviceType(LJM_dtANY), connectionType(LJM_ctANY), outputFile(outFile)
{

	// "logfile.csv", std::ofstream::app

	this->uniqueIdentifier = uniqueIdentifier;
	this->err = LJM_OpenS(devType, connType, iden, &handle);
	ErrorCheck(this->err, "LJM_OpenS");

	char string[LJM_STRING_ALLOCATION_SIZE];

	// Get device name
	this->err = LJM_eReadNameString(this->handle, "DEVICE_NAME_DEFAULT", string);
	if (this->err == LJME_NOERROR)
		printf("DEVICE_NAME_DEFAULT: %s\n", string);
	else
		printf("This device does not have a name\n");

	// Get device info
	this->err = LJM_GetHandleInfo(handle, &deviceType, &connectionType, &serialNumber, &ipAddress,
		&portOrPipe, &packetMaxBytes);
	ErrorCheck(this->err, "LJM_GetHandleInfo");

	this->diagnosticPrint();

	// File Management
	this->outputFile << "computerTime";
	for (int i = 0; i < 9; i++) {
		this->outputFile << "," << this->inputPortNames[i];
	}
	this->outputFile << "\n";

	time(&this->lastCaptureComputerTime);  /* get current time; same as: timer = time(NULL)  */
	
	this->outputFile.flush();
	//this->outputFile.close();

	//this->outputFile.open("out_fle.csv");
										  
	// Create the object's thread at the very end of its constructor
	//TODO
}

// Destructor (Called when object is about to be destroyed
BehavioralBoxLabjack::~BehavioralBoxLabjack()
{
	// Destroy the object's thread at the very start of its destructor
	//TODO

	// Close the open output file:
	this->outputFile.close();

	// Close the connection to the labjack
	CloseOrDie(this->handle);
}

void BehavioralBoxLabjack::diagnosticPrint()
{
	PrintDeviceInfo(deviceType, connectionType, serialNumber, ipAddress, portOrPipe, packetMaxBytes);
	printf("\n");
	GetAndPrint(handle, "HARDWARE_VERSION");
	GetAndPrint(handle, "FIRMWARE_VERSION");
}

int BehavioralBoxLabjack::getError()
{
	return this->err;
}

time_t BehavioralBoxLabjack::getTime()
{
	double labjackTime = 0.0;
	this->err = LJM_eReadAddress(this->handle, 61500, 1, &labjackTime);
	ErrorCheck(this->err, "LJM_eReadAddress");
	return time_t(labjackTime);
}

void BehavioralBoxLabjack::setTime(time_t newTime)
{
	// Write the provided time to the Labjack
		//RTC_SET_TIME_S: address 61504
	this->err = LJM_eWriteAddress(handle, 61504, LJM_UINT32, newTime);
	ErrorCheck(this->err, "LJM_eWriteAddress");
}

void BehavioralBoxLabjack::setVisibleLightRelayState(bool isOn)
{
	// Set up for setting DIO state
	double value = 0; // Output state = low (0 = low, 1 = high)
	char * portName = "MIO2";
	if (isOn) {
		// It's day-time
		value = 0;
	}
	else {
		// It's night-time
		value = 1;
	}
	// Set DIO state on the LabJack
	this->err = LJM_eWriteName(this->handle, portName, value);
	ErrorCheck(this->err, "LJM_eWriteName");
	printf("\nSet %s state : %f\n", portName, value);
}

void BehavioralBoxLabjack::readSensorValues()
{
	time(&this->lastCaptureComputerTime);  /* get current time; same as: timer = time(NULL)  */
	//Read the sensor values from the labjack DIO Inputs
	this->err = LJM_eReadNames(this->handle, 9, (const char **)this->inputPortNames, this->lastReadInputPortValues, &this->errorAddress);
	ErrorCheckWithAddress(this->err, this->errorAddress, "LJM_eReadNames");
	this->persistReadValues();
}

// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
void BehavioralBoxLabjack::persistReadValues()
{
	//printf("readValues: running at %s: ", ctime(&this->lastCaptureComputerTime));
	//this->outputFile << ctime(&this->lastCaptureComputerTime);
	this->outputFile << this->lastCaptureComputerTime;
	for (int i = 0; i < 9; i++) {
		inputPortValuesChanged[i] = (this->lastReadInputPortValues[i] != this->previousReadInputPortValues[i]);
		if (inputPortValuesChanged[i] == true) {
			// The input port changed from the previous value

		}
		//printf(" %s = %.4f  ", this->inputPortNames[i], this->lastReadInputPortValues[i]);
		this->outputFile << "," << int(this->lastReadInputPortValues[i]);

		// After capturing the change, replace the old value
		this->previousReadInputPortValues[i] = this->lastReadInputPortValues[i];
		
	}
	//printf("\n");
	this->outputFile << "\n";
	this->outputFile.flush();
}

