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
#include "../../C_C++_LJM_2019-05-20/LJM_StreamUtilities.h"
const char * labjackStreamPortNames[] = {
"DIO0", "DIO1", "DIO2", "DIO3", "DIO4", "DIO5", "DIO6", "DIO7", "MIO0", "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16", "CORE_TIMER"
};
// 12 ports

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
	for (int i = 0; i < 9; i++) {
		this->csv << this->inputPortNames[i];
	}
	this->csv.writeToFile(fileFullPath, false);

	// wallTime-based event scheduling:
	this->scheduler = new Bosma::Scheduler(max_n_threads);

	// Ran at the top of every hour
	this->scheduler->cron("0 * * * *", [this]() { this->runTopOfHourUpdate(); });
	//scheduler.cron("0 * * * *", []() { runTopOfHourUpdate(); });

	// Ran at the top of every minute
	//this->scheduler.cron("* * * * *", []() { this->runTopOfMinuteUpdate(); });
										  
	DisableStreamIfEnabled(handle);

	this->streamTriggered();

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
	this->csv.writeToFile(this->fileFullPath, true);

	// Turn off any streams:
	DisableStreamIfEnabled(handle);

	// Close the connection to the labjack
	this->err = LJM_Close(this->handle);
	ErrorCheck(this->err, "LJM_Close");

	//CloseOrDie(this->handle);
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
	this->err = LJM_eReadNames(this->handle, 9, (const char **)this->inputPortNames, this->lastReadInputPortValues, &this->errorAddress);
	ErrorCheckWithAddress(this->err, this->errorAddress, "readSensorValues - LJM_eReadNames");
	this->persistReadValues();
}

// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
void BehavioralBoxLabjack::persistReadValues()
{
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();
	CSVWriter newCSVLine(",");

	newCSVLine.newRow() << milliseconds_since_epoch;
	for (int i = 0; i < 9; i++) {
		inputPortValuesChanged[i] = (this->lastReadInputPortValues[i] != this->previousReadInputPortValues[i]);
		if (inputPortValuesChanged[i] == true) {
			// The input port changed from the previous value

		}
		newCSVLine << int(this->lastReadInputPortValues[i]);

		// After capturing the change, replace the old value
		this->previousReadInputPortValues[i] = this->lastReadInputPortValues[i];
		
	}
	newCSVLine.writeToFile(fileFullPath, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
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


void BehavioralBoxLabjack::hardcodedConfigureStream()
{
	const int STREAM_CLOCK_SOURCE = 0;
	const int STREAM_RESOLUTION_INDEX = 0;
	const double STREAM_SETTLING_US = 0;
	const double AIN_ALL_RANGE = 0;
	const int AIN_ALL_NEGATIVE_CH = LJM_GND;

	printf("Writing configurations:\n");

	if (STREAM_CLOCK_SOURCE == 0) {
		printf("    Enabling internally-clocked stream:");
	}
	printf("    Setting STREAM_CLOCK_SOURCE to %d\n", STREAM_CLOCK_SOURCE);
	WriteNameOrDie(this->handle, "STREAM_CLOCK_SOURCE", STREAM_CLOCK_SOURCE);

	// Configure the analog inputs' negative channel, range, settling time and
	// resolution.
	// Note: when streaming, negative channels and ranges can be configured for
	// individual analog inputs, but the stream has only one settling time and
	// resolution.
	printf("    Setting STREAM_RESOLUTION_INDEX to %d\n",
		STREAM_RESOLUTION_INDEX);
	WriteNameOrDie(this->handle, "STREAM_RESOLUTION_INDEX", STREAM_RESOLUTION_INDEX);

	printf("    Setting STREAM_SETTLING_US to %f\n", STREAM_SETTLING_US);
	WriteNameOrDie(this->handle, "STREAM_SETTLING_US", STREAM_SETTLING_US);

	printf("    Setting AIN_ALL_RANGE to %f\n", AIN_ALL_RANGE);
	WriteNameOrDie(this->handle, "AIN_ALL_RANGE", AIN_ALL_RANGE);

	printf("    Setting AIN_ALL_NEGATIVE_CH to ");
	if (AIN_ALL_NEGATIVE_CH == LJM_GND) {
		printf("LJM_GND");
	}
	else {
		printf("%d", AIN_ALL_NEGATIVE_CH);
	}
	printf("\n");
	WriteNameOrDie(this->handle, "AIN_ALL_NEGATIVE_CH", AIN_ALL_NEGATIVE_CH);
}

void BehavioralBoxLabjack::streamTriggered()
{
	// Variables for LJM_eStreamStart
	int * aScanList = (int *)malloc(sizeof(int) * int(NUM_CHANNELS));

	// Variables for LJM_eStreamRead
	unsigned int aDataSize = NUM_CHANNELS * SCANS_PER_READ;
	double * aData = (double *)malloc(sizeof(double) * aDataSize);
	int deviceScanBacklog = 0;
	int LJMScanBacklog = 0;
	int streamRead = 0;
	double value = 0;

	err = LJM_NamesToAddresses(NUM_CHANNELS, labjackStreamPortNames, aScanList, NULL);
	ErrorCheck(err, "Getting positive channel addresses");

	this->hardcodedConfigureStream();

	// Configure LJM for unpredictable stream timing
	SetConfigValue(LJM_STREAM_SCANS_RETURN, LJM_STREAM_SCANS_RETURN_ALL_OR_NONE);
	SetConfigValue(LJM_STREAM_RECEIVE_TIMEOUT_MS, 0);

	// Clear any previous DIO0_EF settings
	WriteNameOrDie(handle, "DIO0_EF_ENABLE", 0);

	printf("Starting stream...\n");
	err = LJM_eStreamStart(handle, SCANS_PER_READ, NUM_CHANNELS, aScanList, &this->actualStreamScanRate);
	ErrorCheck(err, "LJM_eStreamStart");
	printf("Stream started. Actual scan rate: %.02f Hz (%.02f sample rate)\n", this->actualStreamScanRate, this->actualStreamScanRate * NUM_CHANNELS);
	printf("\n");

	// STREAM SYNC with system clock
	// get the CORE_TIMER timestamp when the stream started
	this->err = LJM_eReadName(this->handle, "STREAM_START_TIME_STAMP", &streamStartTimestamp);
	ErrorCheck(this->err, "LJM_eReadName STREAM_START_TIME_STAMP");

	// Read the CORE_TIMER and your system time quickly in a loop for e.g. five times.
	double coreTimerTimes[5] = { 0,0,0,0,0 };
	std::chrono::high_resolution_clock::time_point systemTimes[5];
	for (int i = 0; i < 5; i++) {
		systemTimes[i] = std::chrono::high_resolution_clock::now();
		this->err = LJM_eReadName(this->handle, "CORE_TIMER", &coreTimerTimes[i]);
		ErrorCheck(this->err, "LJM_eReadName CORE_TIMER");
	}
	/*
	You can assume the CORE_TIMER value is, on average, halfway between when you begin the CORE_TIMER read and when you receive the CORE_TIMER value. 
	Throw out any measurements that take an abnormal amount of time. (If most round-trip reads of CORE_TIMER take 1 ms and one CORE_TIMER read takes 3 ms, it's likely that either the outbound or inbound communication had an unusual delay—but unlikely that both had an equal delay.)
	*/
	this->streamStartSystemTime = systemTimes[2];
	double coreTimerDifferrence = coreTimerTimes[2] - this->streamStartTimestamp;
	

	/*
	Next, calculate the CORE_TIMER value for each scan. 
	Use the actual scan rate to calculate the CORE_TIMER value for each scan starting from STREAM_START_TIME_STAMP.
	Once you have your current system clock correlated with CORE_TIMER and each scan is assigned a CORE_TIMER value, you can convert each CORE_TIMER value into a system clock time.
	*/
	//this->actualStreamScanRate



	printf("Stream running (Press ctrl + c to cancel.)\n");
	while (streamRead < NUM_LOOP_ITERATIONS) {
		VariableStreamSleep(SCANS_PER_READ, SCAN_RATE, LJMScanBacklog);

		err = LJM_eStreamRead(handle, aData, &deviceScanBacklog, &LJMScanBacklog);
		if (err == LJME_NO_SCANS_RETURNED) {
			printf(".");
			fflush(stdout);
		}
		else {
			ErrorCheck(err, "LJM_eStreamRead");
			printf("\niteration: %d    ", streamRead);
			int currentReadStartScanAbsoluteIndex = streamRead * SCANS_PER_READ;
			hardcodedPrintScans(labjackStreamPortNames, aData, currentReadStartScanAbsoluteIndex, SCANS_PER_READ, NUM_CHANNELS, deviceScanBacklog, LJMScanBacklog);
			++streamRead;
		}

		err = LJM_eReadName(handle, "STREAM_ENABLE", &value);
		ErrorCheck(err, "LJM_eReadName(%d, STREAM_ENABLE, ...", handle);
		if (!value) {
			printf("\nSTREAM_ENABLE is disabled.\n");
			printf("  This means the device reset or another program disabled stream.\n");
			printf("  Exiting stream read loop now.\n");
			break;
		}
	}

	err = LJM_eStreamStop(handle);
	ErrorCheck(err, "Stopping stream");

	free(aData);
	free(aScanList);

	printf("\nDone with %d iterations\n", NUM_LOOP_ITERATIONS);
}

//currentReadStartScanAbsoluteIndex: number of scans that have been performed prior to this read
void BehavioralBoxLabjack::hardcodedPrintScans(const char ** chanNames, const double * aData, int currentReadStartScanAbsoluteIndex, int numScansReceived, int numChannelsPerScan, int deviceScanBacklog, int LJMScanBacklog)
{
	int dataI, scanI;
	unsigned int timerValue;
	unsigned long long coreTimerValue;
	const int NUM_SCANS_TO_PRINT = numScansReceived;

	if (numChannelsPerScan < NUM_CHANNELS || numChannelsPerScan > NUM_CHANNELS) {
		printf("%s:%d - HardcodedPrintScans() - unexpected numChannelsPerScan: %d\n",
			__FILE__, __LINE__, numChannelsPerScan);
		return;
	}

	printf("devBacklog: % 4d - LJMBacklog: % 4d  - %d of %d scans: \n", deviceScanBacklog, LJMScanBacklog, NUM_SCANS_TO_PRINT, numScansReceived);
	CSVWriter newCSVLine(",");

	for (scanI = 0; scanI < NUM_SCANS_TO_PRINT; scanI++) {
		newCSVLine = 0;
		newCSVLine = CSVWriter(",");

		// Compute the timestamp for the scan
		if (strcmp(chanNames[11], "CORE_TIMER") != 0)
		{
			printf("%s:%d - HardcodedPrintScans() - expected CORE_TIMER register but instead found: %s\n", __FILE__, __LINE__, chanNames[11]);
			return;
		}
		coreTimerValue = (unsigned long long)aData[scanI * 4 + 11];


		//int currentScanAbsoluteIndex = currentReadStartScanAbsoluteIndex + scanI;
		//double scanCoreTimerValue = this->getCoreTimerValueFromScanIndex(currentScanAbsoluteIndex);
		//std::chrono::high_resolution_clock::time_point systemScanTime = this->getSystemTimeFromCoreTimer(scanCoreTimerValue);
		//unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(systemScanTime.time_since_epoch()).count();

		//newCSVLine.newRow() << milliseconds_since_epoch;

		newCSVLine.newRow() << coreTimerValue;
		printf("%d, ", coreTimerValue);

		// Iterate through all 9 channels
		for (dataI = 0; dataI < 9; dataI++) {
			this->lastReadInputPortValues[dataI] = aData[scanI * 4 + dataI];
			inputPortValuesChanged[dataI] = (this->lastReadInputPortValues[dataI] != this->previousReadInputPortValues[dataI]);
			if (inputPortValuesChanged[dataI] == true) {
				// The input port changed from the previous value

			}
			//printf(" % 4.01f (%s),", this->lastReadInputPortValues[dataI], chanNames[dataI]);
			newCSVLine << int(this->lastReadInputPortValues[dataI]); // write out to CSV
			printf("%s:%i, ", chanNames[dataI], int(this->lastReadInputPortValues[dataI]));
			// After capturing the change, replace the old value
			this->previousReadInputPortValues[dataI] = this->lastReadInputPortValues[dataI];
		} // end channels loop

		printf("\n");

		if (strcmp(chanNames[9], "SYSTEM_TIMER_20HZ") != 0
			|| strcmp(chanNames[10], "STREAM_DATA_CAPTURE_16") != 0)
		{
			printf("%s:%d - HardcodedPrintScans() - unexpected register: %s and/or %s\n",
				__FILE__, __LINE__, chanNames[9], chanNames[10]);
			return;
		}

		// Combine SYSTEM_TIMER_20HZ's lower 16 bits and STREAM_DATA_CAPTURE_16, which
		// contains SYSTEM_TIMER_20HZ's upper 16 bits
		timerValue = ((unsigned short)aData[scanI * 4 + 10] << 16) + (unsigned short)aData[scanI * 4 + 9];

		//printf("  0x%8X (%s)", timerValue, chanNames[9]);
		//printf("\n");

		newCSVLine.writeToFile(fileFullPath, false); //TODO: relies on CSV object's internal buffering and writes out to the file each time.

	} // end scan loop

	//newCSVLine.writeToFile(fileFullPath, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
}

double BehavioralBoxLabjack::getCoreTimerValueFromScanIndex(int scanIndex)
{
	double actualCoreTimerOffset = this->actualStreamScanRate * double(scanIndex);
	// Subtract off the stream start time (which is already in a core timer format
	double relativeCoreTimerOffset = actualCoreTimerOffset - this->streamStartTimestamp;
	// TODO: Perform the modulus to compensate for potential CORE_TIMER wrap arounds.
	return relativeCoreTimerOffset;
}

std::chrono::high_resolution_clock::time_point BehavioralBoxLabjack::getSystemTimeFromCoreTimer(double coreTimerValue)
{
	//std::chrono::duration<double, micro>()
	// Each one in the coreTimer represents a tick of the 40MHz clock.
	// 40MHz == 0.025 microseconds
	//std::chrono::duration<double,micro> durationMicrosecondsRep = coreTimerValue * 0.025 * std::chrono::microseconds(1);
	//std::chrono::microseconds microsecondsRep(coreTimerValue)

	//std::chrono::high_resolution_clock::time_point output = this->streamStartSystemTime + durationMicrosecondsRep;
	//std::chrono::high_resolution_clock::time_point output = this->streamStartSystemTime + (0.025 * std::chrono::microseconds(1));
	//std::chrono::high_resolution_clock::time_point<std::chrono::high_resolution_clock, double> output = this->streamStartSystemTime + std::chrono::duration_cast<std::chrono::microseconds>(coreTimerValue * 0.025).count();
	std::chrono::high_resolution_clock::duration spanningDuration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<double, micro>(coreTimerValue * 0.025));
	std::chrono::high_resolution_clock::time_point output = this->streamStartSystemTime + spanningDuration;
	return output;
}
