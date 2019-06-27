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

	
	// Open the LabjackConnection and load some information
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
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();

	// Builds the filename in the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}"
	std::ostringstream os;
	os << "out_file_s" << serialNumber << "_" << milliseconds_since_epoch << ".csv";
	this->filename = os.str();
	// Build the full file path
	if (this->outputDirectory.empty()) {
		this->fileFullPath = this->filename;
	}
	else {
		this->fileFullPath = this->outputDirectory + this->filename;
	}
	std::cout << "New file path: " << this->fileFullPath << std::endl;
	
	// Write the header to the .csv file:
	this->csv.newRow() << "computerTime";
	for (int i = 0; i < NUM_CHANNELS; i++) {
		this->csv << this->inputPortNames[i];
	}
	this->csv.writeToFile(fileFullPath, false);

	this->monitor = new StateMonitor();

	// Create the object's thread at the very end of its constructor
	// wallTime-based event scheduling:
	this->scheduler = new Bosma::Scheduler(max_n_threads);

	// Ran at the top of every hour
	this->scheduler->cron("0 * * * *", [this]() { this->runTopOfHourUpdate(); });

	// Start a 20Hz (50[ms]) loop to read data.
	this->scheduler->every(std::chrono::milliseconds(50), [this]() { this->runPollingLoop(); });
}

// Destructor (Called when object is about to be destroyed
BehavioralBoxLabjack::~BehavioralBoxLabjack()
{
	// Stop the main run loop
	this->shouldStop = true;
	//Read the values and save them one more time, so we know when the end of data collection occured.
	this->readSensorValues();

	// Destroy the object's thread at the very start of its destructor
	delete this->scheduler;
	//this->scheduler = NULL;

	// Close the open output file:
	//this->outputFile.close();
	this->csv.writeToFile(this->fileFullPath, true);

	// Close the connection to the labjack
	this->err = LJM_Close(this->handle);
	ErrorCheck(this->err, "LJM_Close");

	delete this->monitor;
	//CloseOrDie(this->handle);
}

void BehavioralBoxLabjack::diagnosticPrint()
{
	PrintDeviceInfo(deviceType, connectionType, serialNumber, ipAddress, portOrPipe, packetMaxBytes);
	printf("\n");
	GetAndPrint(handle, "HARDWARE_VERSION");
	GetAndPrint(handle, "FIRMWARE_VERSION");
}

void BehavioralBoxLabjack::diagnosticPrintLastValues()
{
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();
	std::cout << milliseconds_since_epoch;
	for (int i = 0; i < NUM_CHANNELS; i++) {
		//if (inputPortValuesChanged[i] == true) {
		//	// The input port changed from the previous value

		//}
		std::cout << "\t" << this->lastReadInputPortValues[i];
	}
	std::cout << std::endl;
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

double BehavioralBoxLabjack::syncDeviceTimes()
{
	int LJMError;
	time_t originalLabjackTime = this->getTime();
	LJMError = this->getError();
	printf("LABJACK TIME: %s\n", ctime(&originalLabjackTime));

	// Get the computer time:
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("COMPUTER TIME: %s\n", ctime(&computerTime));

	double updateChangeSeconds = difftime(computerTime, originalLabjackTime);

	if (updateChangeSeconds == 0) {
		printf("Computer time is already synced with Labjack time!\n");
	}
	else {
		printf("Computer time is %.f seconds from Labjack time...\n", updateChangeSeconds);
		// Write the computer time to the Labjack
		this->setTime(computerTime);
		LJMError = this->getError();

		// Re-read the time to confirm the update
		time_t updatedLabjackTime = this->getTime();
		LJMError = this->getError();
		printf("Updated Labjack TIME: %s\n", ctime(&updatedLabjackTime));
	}
	return updateChangeSeconds;
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
	this->err = LJM_eReadNames(this->handle, NUM_CHANNELS, (const char **)this->inputPortNames, this->lastReadInputPortValues, &this->errorAddress);
	ErrorCheckWithAddress(this->err, this->errorAddress, "readSensorValues - LJM_eReadNames");
	// Only persist the values if the state has changed.
	if (this->monitor->refreshState(this->lastCaptureComputerTime, this->lastReadInputPortValues)) {
		this->persistReadValues(true);
	}
}

// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
void BehavioralBoxLabjack::persistReadValues(bool enableConsoleLogging)
{
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();
	CSVWriter newCSVLine(",");

	if (enableConsoleLogging) {
		cout << milliseconds_since_epoch << ": ";
	}
	newCSVLine.newRow() << milliseconds_since_epoch;
	for (int i = 0; i < NUM_CHANNELS; i++) {
		inputPortValuesChanged[i] = (this->lastReadInputPortValues[i] != this->previousReadInputPortValues[i]);
		if (inputPortValuesChanged[i] == true) {
			// The input port changed from the previous value

		}
		newCSVLine << this->lastReadInputPortValues[i];
		if (enableConsoleLogging) {
			cout << this->lastReadInputPortValues[i] << ", ";
		}
		// After capturing the change, replace the old value
		this->previousReadInputPortValues[i] = this->lastReadInputPortValues[i];
		
	}
	if (enableConsoleLogging) {
		cout << std::endl;
	}
	newCSVLine.writeToFile(fileFullPath, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
}


// The main run loop
void BehavioralBoxLabjack::runPollingLoop()
{
	if (this->shouldStop) {
		// Stop running the main loop!
		printf("Stopping Labjack %d", this->uniqueIdentifier);
	}
	else {
		this->readSensorValues();
	}
}

// Executed every hour, on the hour
void BehavioralBoxLabjack::runTopOfHourUpdate()
{
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("runTopOfHourUpdate: running at %s for labjack %i\n", ctime(&computerTime), this->serialNumber);
	this->updateVisibleLightRelayIfNeeded();
}

bool BehavioralBoxLabjack::isArtificialDaylightHours()
{
	time_t currTime = time(NULL);
	struct tm *currLocalTime = localtime(&currTime);

	int hour = currLocalTime->tm_hour;
	if ((hour < 6) || (hour > 18)) {
		// It's night-time
		return false;
	}
	else {
		// It's day-time
		return true;
	}
}

void BehavioralBoxLabjack::updateVisibleLightRelayIfNeeded()
{
	bool isDay = isArtificialDaylightHours();
	this->setVisibleLightRelayState(isDay);
}