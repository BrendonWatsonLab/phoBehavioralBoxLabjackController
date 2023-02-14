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
#include "OutputState.h"
#include "ConfigurationManager.h"

class LabjackLogicalInputChannel;

typedef std::chrono::system_clock Clock;

//// Scheduler
#include "External/Scheduler/Scheduler.h"

// REGEX:
static const std::regex behavioral_box_labjack_deviceName_regex("LJ-(\\d{2})");


//#include "LabjackStreamHelpers.h"
#include "LabjackStreamInfo.h"


class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, int serialNumber);
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, int serialNumber);
	BehavioralBoxLabjack(const BehavioralBoxLabjack&) = delete; // non construction-copyable
	BehavioralBoxLabjack& operator=(const BehavioralBoxLabjack&) = delete; // non copyable
	~BehavioralBoxLabjack();

	double* lastReadExpandedPortValues = nullptr;// David trying to make a global ass pointer with the worst coding background ever seen to mankind

	enum class PortEnumerationMode { logicalChannelOnly, portNames, expandedPortNames };
	/*
	 * logicalChannelOnly: only the logical channel name, like {"DISPENSE_SIGNALS"}
	 * portNames: only the real port names, like {"EIO_STATE"}
	 * expandedPortNames: the expanded port names, like {"EIO0", "EIO1", "EIO2", "EIO3", ...}
	 */

	void diagnosticPrint();
	void printIdentifierLine();
	//void diagnosticPrintLastValues();
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
	

	//void persistReadValues(bool enableConsoleLogging);
	//void persistReadValues();

	// Main run loop
	void runPollingLoop();

	// Getters:
	int getSerialNumber() { return this->serialNumber; }
	std::string getDeviceName() { return this->deviceName; }
	bool isVisibleLEDLit();


	// CSV Output File Access:
	std::string getFullFilePath() { return this->fileFullPath; }

	
	int getNumberInputChannels(PortEnumerationMode port_enumeration_mode, bool include_digital_ports = true, bool include_analog_ports = false);
	//int getNumberInputChannels(bool include_digital_ports = true, bool include_analog_ports = false);

	std::vector<std::string> getInputPortNames(PortEnumerationMode port_enumeration_mode, bool include_digital_ports = true, bool include_analog_ports = false);

	
	int getNumberOutputChannels() { return NUM_OUTPUT_CHANNELS; }
	std::string getOutputDirectory() { return this->outputDirectory; }

	LabjackStreamInfo getStreamInfo() { return this->ljStreamInfo; }

	// Override Functions
	void toggleOverrideMode_VisibleLED();
	void toggleOverrideMode_AttractModeLEDs();


	// Save out configuration File (INI)
	bool saveConfigurationFile(std::string filePath);

	

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
	//bool DavidsAnalogWriterBlock = true; // this is to control the write speed of the analog data. It makes performPersistValues function only write half the speed of the sampling frequency
	int analogWriterCounter = 0;

	// Override Values: 
	bool isOverrideActive_VisibleLED = false;
	bool overrideValue_isVisibleLEDLit = false;

	bool isOverrideActive_AttractModeLEDs = false;
	bool overrideValue_areAttractModeLEDsLit = false;


	std::shared_ptr<ConfigurationManager> configMan = std::make_shared<ConfigurationManager>();

	
	// File Output:
	CSVWriter csv;
	std::string filename = "outputFile.csv";
	std::string outputDirectory = configMan->getGeneratedActiveOutputDirectory(); // should end in a slash if it's not empty
	std::string fileFullPath = "C:/Common/data/outputFile.csv";

	CSVWriter csv_analog;
	std::string filename_analog = "outputFile_analog.csv";
	std::string fileFullPath_analog = "C:/Common/data/outputFile_analog.csv";


	// Labjack Stream-specific values:
	//StreamInfo ljStreamInfo;
	/*std::shared_ptr<LabjackStreamInfo> ljStreamInfo = std::make_shared<LabjackStreamInfo>();*/
	/*std::shared_ptr<LabjackStreamInfo> ljStreamInfo;*/
	LabjackStreamInfo ljStreamInfo;

	std::vector<LabjackLogicalInputChannel*> logicalInputChannels = {};

	void testBuildLogicalInputChannels();
	

	// Variables for holding the last read values
	void LoadActiveLogicalInputChannelsConfig();
	

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


	//void performPersistValues(unsigned long long estimated_scan_milliseconds_since_epoch, double* lastReadValues, bool did_anyAnalogPortChange,	bool did_anyDigitalPortChange, bool enableConsoleLogging);

	void performPersistValues(unsigned long long estimated_scan_milliseconds_since_epoch, std::vector<std::vector<double>> newestReadValues, bool did_anyAnalogPortChange, bool did_anyDigitalPortChange, bool enableConsoleLogging);



	// Stream Functionality:
	void SetupStream();
	
	/**
	 * Prints scans of the channels:
	 *     "AIN0",  "FIO_STATE",  "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16".
	 * Combines SYSTEM_TIMER_20HZ and STREAM_DATA_CAPTURE_16 to create the original
	 * 32-bit value of SYSTEM_TIMER_20HZ.
	**/

};


