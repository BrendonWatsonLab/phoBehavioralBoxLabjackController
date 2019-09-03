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

#include "External/C_C++_LJM/LJM_Utilities.h"
//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"

BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, int serialNumber) : BehavioralBoxLabjack(uniqueIdentifier, NumberToDeviceType(devType), NumberToConnectionType(connType), serialNumber) {}

// Constructor: Called when an instance of the object is about to be created
BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, int serialNumber): deviceType(LJM_dtANY), connectionType(LJM_ctANY), csv(CSVWriter(",")), lastCaptureComputerTime(Clock::now())
{
	this->serialNumber = serialNumber;
	char iden[256];
	sprintf(iden, "%d", this->serialNumber);
	
	// Open the LabjackConnection and load some information
	this->uniqueIdentifier = uniqueIdentifier;
	this->err = LJM_OpenS(devType, connType, iden, &this->handle);
	this->printIdentifierLine();
	ErrorCheck(this->err, "LJM_OpenS");

	char string[LJM_STRING_ALLOCATION_SIZE];

	// Get device name
	this->err = LJM_eReadNameString(this->handle, "DEVICE_NAME_DEFAULT", string);
	if (this->err == LJME_NOERROR)
		printf("\t DEVICE_NAME_DEFAULT: %s\n", string);
	else
		printf("\t This device does not have a name\n");

	// Get device info
	this->err = LJM_GetHandleInfo(this->handle, &deviceType, &connectionType, &this->serialNumber, &ipAddress,
		&portOrPipe, &packetMaxBytes);
	ErrorCheck(this->err, "LJM_GetHandleInfo");

	//this->diagnosticPrint();

	// File Management
	// Build the file name
	this->lastCaptureComputerTime = Clock::now();
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();

	// Builds the filename in the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}"
	std::ostringstream os;
	os << "out_file_s" << this->serialNumber << "_" << milliseconds_since_epoch << ".csv";
	this->filename = os.str();
	// Build the full file path
	if (this->outputDirectory.empty()) {
		this->fileFullPath = this->filename;
	}
	else {
		this->fileFullPath = this->outputDirectory + this->filename;
	}
	std::cout << "\t New file path: " << this->fileFullPath << std::endl;
	
	// Write the header to the .csv file:
	this->csv.newRow() << "computerTime";
	for (int i = 0; i < NUM_CHANNELS; i++) {
		this->csv << this->inputPortNames[i];
	}
	this->csv.writeToFile(fileFullPath, false);

	// Setup output ports states:
	this->water1PortEndIlluminationTime = Clock::now();
	this->water2PortEndIlluminationTime = Clock::now();

	std::function<double()> visibleLEDRelayFunction = [=]() -> double {
		if (this->isVisibleLEDLit()) { return 0.0; }
		else { return 1.0; }
	};
	//std::function<double(int)> drinkingPortAttractorModeFunction = [=](int portNumber) -> double {
	//	if (!this->isAttractModeLEDLit(portNumber)) { return 0.0; }
	//	else { return 1.0; }
	//};

	// Create output ports for all output ports (TODO: make dynamic)
	for (int i = 0; i < NUM_OUTPUT_CHANNELS; i++) {
		std::string portName = std::string(outputPortNames[i]);
		OutputState* currOutputPort;
		if (i == 0) {
			currOutputPort = new OutputState(portName, visibleLEDRelayFunction);
		}
		else {
			std::function<double()> drinkingPortAttractorModeFunction = [=]() -> double {
				if (!this->isAttractModeLEDLit(i)) { return 0.0; }
				else { return 1.0; }
			};
			currOutputPort = new OutputState(portName, drinkingPortAttractorModeFunction);
		}
		
		this->outputPorts.push_back(currOutputPort);
	}
	//TODO: force initializiation

	// Setup input state 
	this->monitor = new StateMonitor();

	// Create the object's thread at the very end of its constructor
	// wallTime-based event scheduling:
	this->scheduler = new Bosma::Scheduler(max_n_threads);

	// Ran at the top of every hour
	this->scheduler->cron("0 * * * *", [this]() { this->runTopOfHourUpdate(); });

	// Start a 20Hz (50[ms]) loop to read data.
	this->scheduler->every(std::chrono::milliseconds(LABJACK_UPDATE_LOOP_FREQUENCY_MILLISEC), [this]() { this->runPollingLoop(); });
}

// Destructor (Called when object is about to be destroyed
BehavioralBoxLabjack::~BehavioralBoxLabjack()
{
	// Stop the main run loop
	this->shouldStop = true;
	//Read the values and save them one more time, so we know when the end of data collection occured.
	this->readSensorValues();

	// Cleanup output ports vector
	for (int i = 0; i < NUM_OUTPUT_CHANNELS; i++) {
		delete this->outputPorts[i];
	}

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
	this->printIdentifierLine();
	PrintDeviceInfo(deviceType, connectionType, serialNumber, ipAddress, portOrPipe, packetMaxBytes);
	printf("\n");
	GetAndPrint(handle, "HARDWARE_VERSION");
	GetAndPrint(handle, "FIRMWARE_VERSION");
}

// Prints the line that uniquely identifies this labjack
void BehavioralBoxLabjack::printIdentifierLine()
{
	cout << ">> Labjack [" << this->serialNumber << "] :" << endl;
}

void BehavioralBoxLabjack::diagnosticPrintLastValues()
{
	this->printIdentifierLine();
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();
	std::cout << "\t " << milliseconds_since_epoch;
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

	this->printIdentifierLine();
	printf("\t LABJACK TIME: %s", ctime(&originalLabjackTime));
	// Get the computer time:
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("\t COMPUTER TIME: %s", ctime(&computerTime));

	double updateChangeSeconds = difftime(computerTime, originalLabjackTime);

	if (updateChangeSeconds == 0) {
		printf("\t Computer time is already synced with Labjack time!\n");
	}
	else {
		printf("\t Computer time is %.f seconds from Labjack time...", updateChangeSeconds);
		// Write the computer time to the Labjack
		this->setTime(computerTime);
		LJMError = this->getError();

		// Re-read the time to confirm the update
		time_t updatedLabjackTime = this->getTime();
		LJMError = this->getError();
		printf("\t Updated Labjack TIME: %s\n", ctime(&updatedLabjackTime));
	}
	return updateChangeSeconds;
}

void BehavioralBoxLabjack::writeOutputPinValues()
{
	this->writeOutputPinValues(false);
}
void BehavioralBoxLabjack::writeOutputPinValues(bool shouldForceWrite)
{
	auto writeTime = Clock::now();

	//Loop through and write the values that have changed
	// Iterate through the output ports
	for (int i = 0; i < outputPorts.size(); i++)
	{
		// Get the appropriate value for the current port (calculating from the saved lambda function).
		double outputValue = outputPorts[i]->getValue();

		// Check to see if the value changed, and if it did, write it.
		bool didChange = outputPorts[i]->set(writeTime, outputValue);

		if (didChange || shouldForceWrite) {
			// Get the c_string name to pass to the labjack write function
			std::string portNameString = outputPorts[i]->getPinName();
			const char* portName = portNameString.c_str();

			// Set DIO state on the LabJack
			//TODO: the most general way to handle this would be to have each pin have a lambda function stored that sets the value in an appropriate way. This is currently a workaround. 
			bool isVisibleLightRelayPort = (i == 0);
			if (isVisibleLightRelayPort) {
				if (outputValue == 0.0) {
					// a value of 0.0 means that the light should be on, so it should be in output mode.
					this->err = LJM_eWriteName(this->handle, portName, outputValue);
					ErrorCheck(this->err, "LJM_eWriteName");
				}
				else {
					// a value greater than 0.0 means that the light should be off, so it should be set to input mode. This is accomplished by reading from the port (instead of writing).
					double tempReadValue = 0.0;
					this->err = LJM_eReadName(this->handle, portName, &tempReadValue);
					ErrorCheck(this->err, "LJM_eReadName");
					//cout << "\t Visible LED Override: read from port. " << tempReadValue << endl;
				}
			}
			else {
				// Not the visible light relay and can be handled in the usual way.
				this->err = LJM_eWriteName(this->handle, portName, outputValue);
				ErrorCheck(this->err, "LJM_eWriteName");
			}
			if (PRINT_OUTPUT_VALUES_TO_CONSOLE) {
				printf("\t Set %s state : %f\n", portName, outputValue);
			}
			
		}
	} // end for
}

void BehavioralBoxLabjack::readSensorValues()
{
	this->lastCaptureComputerTime = Clock::now();

	//Read the sensor values from the labjack DIO Inputs
	this->err = LJM_eReadNames(this->handle, NUM_CHANNELS, (const char **)this->inputPortNames, this->lastReadInputPortValues, &this->errorAddress);
	ErrorCheckWithAddress(this->err, this->errorAddress, "readSensorValues - LJM_eReadNames");
	// Only persist the values if the state has changed.
	if (this->monitor->refreshState(this->lastCaptureComputerTime, this->lastReadInputPortValues)) {
		//TODO: should this be asynchronous? This would require passing in the capture time and read values
		this->persistReadValues(true);
	}
}

// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
void BehavioralBoxLabjack::persistReadValues(bool enableConsoleLogging)
{
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();
	// Lock the mutex to prevent concurrent persisting
	std::lock_guard<std::mutex> csvLock(this->logMutex);
	CSVWriter newCSVLine(",");

	if (enableConsoleLogging) {
		this->printIdentifierLine();
		cout << "\t " << milliseconds_since_epoch << ": ";
	}
	newCSVLine.newRow() << milliseconds_since_epoch;
	for (int i = 0; i < NUM_CHANNELS; i++) {
		inputPortValuesChanged[i] = (this->lastReadInputPortValues[i] != this->previousReadInputPortValues[i]);
		if (inputPortValuesChanged[i] == true) {
			// The input port changed from the previous value
			// Special handling for the water ports. If the port is a water port that has transitioned from off to on, set the appropriate "this->water*PortEndIlluminationTime" variable so the port is illuminated for a second after dispense.
			if (this->lastReadInputPortValues[i] > 0.0) {
				// If the port transitioned from off to on:
				if (strcmp(this->inputPortPurpose[i], "Water1_BeamBreak") == 0) {
					auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(this->lastCaptureComputerTime);
					this->water1PortEndIlluminationTime = Clock::now() + std::chrono::seconds(1);
				}
				else if (strcmp(this->inputPortPurpose[i], "Water2_BeamBreak") == 0) {
					this->water2PortEndIlluminationTime = Clock::now() + std::chrono::seconds(1);
				}
			} // end if greater than zero

#if LAUNCH_WEB_SERVER
			// Emit the "valueChanged" signal for the web server
			this->valueChanged_.emit(this->serialNumber, i, this->lastReadInputPortValues[i]);
#endif // LAUNCH_WEB_SERVER

		} // end if input port values changed
		newCSVLine << this->lastReadInputPortValues[i];
		if (enableConsoleLogging) {
			cout << this->lastReadInputPortValues[i] << ", ";
		}
		// After capturing the change, replace the old value
		this->previousReadInputPortValues[i] = this->lastReadInputPortValues[i];
	} //end for num channels
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
		this->writeOutputPinValues();
	}
}

bool BehavioralBoxLabjack::isVisibleLEDLit()
{
	if (this->isOverrideActive_VisibleLED) {
		return this->overrideValue_isVisibleLEDLit;
	}
	else {
		return this->isArtificialDaylightHours();
	}
}


vector<std::string> BehavioralBoxLabjack::getInputPortNames()
{
	vector<std::string> outputStrings = vector<std::string>();
	std::string currString = "";
	for (int i = 0; i < this->getNumberInputChannels(); i++) {
		currString = std::string(this->inputPortNames[i]);
		outputStrings.push_back(currString);
	}
	return outputStrings;
}

vector<std::string> BehavioralBoxLabjack::getInputPortPurpose()
{
	vector<std::string> outputStrings = vector<std::string>();
	std::string currString = "";
	for (int i = 0; i < this->getNumberInputChannels(); i++) {
		currString = std::string(this->inputPortPurpose[i]);
		outputStrings.push_back(currString);
	}
	return outputStrings;
}

vector<double> BehavioralBoxLabjack::getLastReadValues()
{
	vector<double> outputValues = vector<double>();
	for (int i = 0; i < this->getNumberInputChannels(); i++) {
		outputValues.push_back(this->lastReadInputPortValues[i]);
	}
	return outputValues;
}

void BehavioralBoxLabjack::toggleOverrideMode_VisibleLED()
{
	// Mode 0: 0 0
	// Mode 1: 1 0
	// Mode 2: 1 1
	if (this->isOverrideActive_VisibleLED) {
		// Override mode is already active (modes 1 or mode 2)
		if (this->overrideValue_isVisibleLEDLit) {
			// If the LED is already lit (mode 2), transition to (mode 0)
			this->isOverrideActive_VisibleLED = false;
			this->overrideValue_isVisibleLEDLit = false;
			cout << "\t Override<" << "Visible LED" << ">" << "Mode 0: Light Default Behavior" << endl;
		}
		else {
			// Otherwise if the LED is in (mode 1), transition to (mode 2)
			this->overrideValue_isVisibleLEDLit = true;
			this->isOverrideActive_VisibleLED = true;
			cout << "\t Override<" << "Visible LED" << ">" << "Mode 2: Light Forced ON" << endl;
		}
	}
	else {
		// Override mode isn't active (mode 0), transition to (mode 1)
		this->overrideValue_isVisibleLEDLit = false;
		this->isOverrideActive_VisibleLED = true;
		cout << "\t Override<" << "Visible LED" << ">" << "Mode 1: Light Forced OFF" << endl;
	}
}

void BehavioralBoxLabjack::toggleOverrideMode_AttractModeLEDs()
{
	// Mode 0: 0 0
	// Mode 1: 1 0
	// Mode 2: 1 1
	if (this->isOverrideActive_AttractModeLEDs) {
		// Override mode is already active (modes 1 or mode 2)
		if (this->overrideValue_areAttractModeLEDsLit) {
			// If the LED is already lit (mode 2), transition to (mode 0)
			this->isOverrideActive_AttractModeLEDs = false;
			this->overrideValue_areAttractModeLEDsLit = false;
			cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 0: LEDs Default Behavior" << endl;
		}
		else {
			// Otherwise if the LED is in (mode 1), transition to (mode 2)
			this->overrideValue_areAttractModeLEDsLit = true;
			this->isOverrideActive_AttractModeLEDs = true;
			cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 2: LEDs Forced ON" << endl;
		}
	}
	else {
		// Override mode isn't active (mode 0), transition to (mode 1)
		this->overrideValue_areAttractModeLEDsLit = false;
		this->isOverrideActive_AttractModeLEDs = true;
		cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 1: LEDs Forced OFF" << endl;
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
	// Note this has been changed as of 8/16/2019.
	// globalDaylightStartHour: defines the hour of the day at which the Visible LEDS are turned on (illuminated) (simulating daylight for the mouse).
	// globalDaylightOffHour: defines the hour of the day at which the Visible LEDS are turned off (simulating nighttime for the mouse).
	if ((hour < globalDaylightStartHour) || (hour >= globalDaylightOffHour)) {
		// It's night-time
		return false;
	}
	else {
		// It's day-time
		return true;
	}
}

bool BehavioralBoxLabjack::isAttractModeLEDLit(int portNumber)
{
	if (this->isOverrideActive_AttractModeLEDs) {
		return this->overrideValue_areAttractModeLEDsLit;
	}
	else {
		// No overrides active (in default behavior mode (mode 0))
		auto currentTime = Clock::now();
		if (portNumber == 1) {
			if ((currentTime <= this->water1PortEndIlluminationTime)) {
				return true;
			}
			else {
				return false;
			}
		}
		else if (portNumber == 2) {
			if ((currentTime <= this->water2PortEndIlluminationTime)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
}

void BehavioralBoxLabjack::updateVisibleLightRelayIfNeeded()
{
	this->writeOutputPinValues(true);
	//bool isDay = isArtificialDaylightHours();
	//this->setVisibleLightRelayState(isDay);
}