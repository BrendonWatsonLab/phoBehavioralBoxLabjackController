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
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"



BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, const char * iden) : BehavioralBoxLabjack(uniqueIdentifier, NumberToDeviceType(devType), NumberToConnectionType(connType), iden) {}

// Constructor: Called when an instance of the object is about to be created
BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden): deviceType(LJM_dtANY), connectionType(LJM_ctANY), csv(CSVWriter(",")), lastCaptureComputerTime(Clock::now())
{

	// "logfile.csv", std::ofstream::app
	//this->csv = CSVWriter(",");

	this->uniqueIdentifier = uniqueIdentifier;
	this->err = LJM_OpenS(devType, connType, iden, &this->handle);
	ErrorCheck(this->err, "LJM_OpenS");

	char string[LJM_STRING_ALLOCATION_SIZE];

	// Get device name
	this->err = LJM_eReadNameString(this->handle, "DEVICE_NAME_DEFAULT", string);
	if (this->err == LJME_NOERROR)
		printf("DEVICE_NAME_DEFAULT: %s\n", string);
	else
		printf("This device does not have a name\n");

	// Get device info
	this->err = LJM_GetHandleInfo(this->handle, &deviceType, &connectionType, &serialNumber, &ipAddress,
		&portOrPipe, &packetMaxBytes);
	ErrorCheck(this->err, "LJM_GetHandleInfo");

	this->diagnosticPrint();

	// File Management
	// Build the file name
	this->lastCaptureComputerTime = Clock::now();
	auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(this->lastCaptureComputerTime);
	auto fraction = this->lastCaptureComputerTime - seconds;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
	//time_t cnow = Clock::to_time_t(this->lastCaptureComputerTime);
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();

	std::ostringstream os;
	os << "out_file_" << uniqueIdentifier << "_" << milliseconds_since_epoch << ".csv";
	this->filename = os.str();
	// Build the full file path
	if (this->outputDirectory.empty()) {
		this->fileFullPath = this->filename;
	}
	else {
		this->fileFullPath = this->outputDirectory + this->filename;
	}
	
	// Write the header to the .csv file:
	//this->outputFile << "computerTime";
	this->csv.newRow() << "computerTime";
	for (int i = 0; i < 9; i++) {
		//this->outputFile << "," << this->inputPortNames[i];
		this->csv << this->inputPortNames[i];
	}
	//this->outputFile << std::endl;
	//this->csv << '\n';
	this->csv.writeToFile(fileFullPath, true);

	// Get the current times
	//time(&this->lastCaptureComputerTime);  /* get current time; same as: timer = time(NULL)  */
	

	//ctime

	//this->outputFile.flush();
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
	//this->outputFile.close();
	this->csv.writeToFile(fileFullPath, true);

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
	ErrorCheck(this->err, "getTime - LJM_eReadAddress");
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
	//time(&this->lastCaptureComputerTime);  /* get current time; same as: timer = time(NULL)  */
	this->lastCaptureComputerTime = Clock::now();

	//Read the sensor values from the labjack DIO Inputs
	this->err = LJM_eReadNames(this->handle, 9, (const char **)this->inputPortNames, this->lastReadInputPortValues, &this->errorAddress);
	ErrorCheckWithAddress(this->err, this->errorAddress, "readSensorValues - LJM_eReadNames");
	this->persistReadValues();
}

// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
void BehavioralBoxLabjack::persistReadValues()
{
	//printf("readValues: running at %s: ", ctime(&this->lastCaptureComputerTime));
	//this->outputFile << ctime(&this->lastCaptureComputerTime);
	//this->lastCaptureComputerTime.time_since_epoch;
	//time_t cnow = Clock::to_time_t(this->lastCaptureComputerTime);
	//tm* localTime = localtime(cnow&);
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();

	//localTime
	//std::ctime(&t)


	this->csv.newRow() << milliseconds_since_epoch;
	for (int i = 0; i < 9; i++) {
		inputPortValuesChanged[i] = (this->lastReadInputPortValues[i] != this->previousReadInputPortValues[i]);
		if (inputPortValuesChanged[i] == true) {
			// The input port changed from the previous value

		}
		//printf(" %s = %.4f  ", this->inputPortNames[i], this->lastReadInputPortValues[i]);
		//this->outputFile << "," << int(this->lastReadInputPortValues[i]);
		this->csv << int(this->lastReadInputPortValues[i]);

		// After capturing the change, replace the old value
		this->previousReadInputPortValues[i] = this->lastReadInputPortValues[i];
		
	}
	//printf("\n");
	//this->csv << "\n";
	this->csv.writeToFile(fileFullPath, true);
}

