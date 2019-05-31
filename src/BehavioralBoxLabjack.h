/**
 * Name: BehavioralBoxLabjack.h
 * Desc: An object representing a single labjack used inside a behavioral box
 // Want to read in sensor values in Labjack's "Command-response" mode for minimum latency
**/
#pragma once
#include <time.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include "External/CSVWriter.h"

typedef std::chrono::system_clock Clock;

//// Scheduler
#include "External/Scheduler/Scheduler.h"
// number of tasks that can run simultaneously
#define max_n_threads 1
//unsigned int max_n_threads = 3;


// STREAM:
#define SCAN_RATE 20 //In Hz
const int SCANS_PER_READ = SCAN_RATE / 2;
//enum { NUM_CHANNELS = 12 };
enum { NUM_CHANNELS = 9 };

// Because SYSTEM_TIMER_20HZ is a 32-bit value and stream can only collect
// 16-bit values per channel, STREAM_DATA_CAPTURE_16 is used to capture the
// final 16 bits of SYSTEM_TIMER_20HZ. See HardcodedPrintScans().
//const char * POS_NAMES[] = {
//	"AIN0",  "FIO_STATE",  "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"
//};
//const char * labjackStreamPortNames[] = {
//"AIN0",  "FIO_STATE",  "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"
//};
const int NUM_LOOP_ITERATIONS = 100;

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
	char * inputPortNames[NUM_CHANNELS] = {"DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0"};


	double previousReadInputPortValues[NUM_CHANNELS] = {0,0,0,0,0,0,0,0,0};
	double lastReadInputPortValues[NUM_CHANNELS] = { 0,0,0,0,0,0,0,0,0};
	bool inputPortValuesChanged[NUM_CHANNELS] = {false, false, false, false, false, false, false, false, false};
	int errorAddress;

	// Time Keeping
	std::chrono::time_point<Clock> lastCaptureComputerTime;
	Bosma::Scheduler* scheduler; // For wallTime based scheduling

	// Scheduled tasks
	void runTopOfHourUpdate(); // Runs at the top of every hour (exactly on the hour, according to system time).
	//void runTopOfMinuteUpdate();

	// Visible Light Relay Control
	void setVisibleLightRelayState(bool isOn);

	// Stream:
	double streamStartTimestamp = 0;
	std::chrono::high_resolution_clock::time_point streamStartSystemTime;
	double actualStreamScanRate = SCAN_RATE;
	void hardcodedConfigureStream();
	void streamTriggered();
	void hardcodedPrintScans(const char ** chanNames, const double * aData, int currentReadStartScanAbsoluteIndex, int numScansReceived, int numChannelsPerScan, int deviceScanBacklog, int LJMScanBacklog);

	double getCoreTimerValueFromScanIndex(int scanIndex);
	std::chrono::high_resolution_clock::time_point getSystemTimeFromCoreTimer(double coreTimerValue);

};

