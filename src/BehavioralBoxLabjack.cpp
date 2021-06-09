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
#include "External/C_C++_LJM/LJM_StreamUtilities.h" // Include the Stream utilities now

#include "FilesystemHelpers.h"
#include "LabjackHelpers.h"
//#include "LabjackStreamHelpers.h"
//#include "LabjackStreamInfo.h"

#include "LabjackLogicalInputChannel.h"

// Set to non-zero for external stream clock
#define EXTERNAL_STREAM_CLOCK 0

// Set FIO0 to pulse out. See EnableFIO0PulseOut()
#define FIO0_PULSE_OUT 0


BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, int devType, int connType, int serialNumber) : BehavioralBoxLabjack(uniqueIdentifier, NumberToDeviceType(devType), NumberToConnectionType(connType), serialNumber) {}

// Constructor: Called when an instance of the object is about to be created
BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, int serialNumber): deviceType(LJM_dtANY), connectionType(LJM_ctANY), csv(CSVWriter(",")), csv_analog(CSVWriter(",")), lastCaptureComputerTime(Clock::now()), ljStreamInfo(LabjackStreamInfo())
{
	// Starts by building the new port objects
	this->testBuildLogicalInputChannels();
	
	/*for (int i = 0; i < NUM_CHANNELS; i++)
	{
		this->logicalInputChannels = new LabjackLogicalInputChannel()
	}*/

	
	this->serialNumber = serialNumber;
	char iden[256];
	sprintf(iden, "%d", this->serialNumber);
	
	// Open the LabjackConnection and load some information
	this->uniqueIdentifier = uniqueIdentifier;
	this->err = LJM_OpenS(devType, connType, iden, &this->handle);
	this->printIdentifierLine();
	ErrorCheck(this->err, "LJM_OpenS");

	char string[LJM_STRING_ALLOCATION_SIZE];

	this->initializeLabjackConfigurationIfNeeded();

	// Get device info
	this->err = LJM_GetHandleInfo(this->handle, &deviceType, &connectionType, &this->serialNumber, &ipAddress,
		&portOrPipe, &packetMaxBytes);
	ErrorCheck(this->err, "LJM_GetHandleInfo");

	//this->diagnosticPrint();

	// File Management
	// Build the file name
	this->lastCaptureComputerTime = Clock::now();
	unsigned long long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(this->lastCaptureComputerTime.time_since_epoch()).count();

	// Builds the digital filename in the form "out_file_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}"
	std::ostringstream os;
	os << "out_file_s" << this->serialNumber << "_" << milliseconds_since_epoch << ".csv";
	this->filename = os.str();

	// Builds the analog filename in the form "out_file_analog_s{SERIAL_NUMBER}_{MILLISECONDS_SINCE_EPOCH}"
	std::ostringstream os_analog;
	os_analog << "out_file_analog_s" << this->serialNumber << "_" << milliseconds_since_epoch << ".csv";
	this->filename_analog = os_analog.str();

	// Build the full file paths
	if (this->outputDirectory.empty()) {
		this->fileFullPath = this->filename;
		this->fileFullPath_analog = this->filename_analog;
	}
	else {
		// Create the output directories if they don't exist.
		bool wasDirectoryCreated = FilesystemHelpers::createDirectory(this->outputDirectory);
		if (wasDirectoryCreated) {
			std::cout << "Directory " << this->outputDirectory << " did not exist. It was created." << std::endl;
		}
		this->fileFullPath = this->outputDirectory + this->filename;
		this->fileFullPath_analog = this->outputDirectory + this->filename_analog;
	}
	std::cout << "\t New file paths: " << std::endl << "\t Digital Inputs: " << this->fileFullPath << std::endl << "\t Analog Inputs: " << this->fileFullPath_analog << std::endl;
	
	// Write the header to the digital .csv file:
	//FIXME: Need to perform a smarter csv header line generation to deal with digital bit arrays and analog ports read as binary
	/*
	 * The same logic present in the new persistReadValues(...) replacement should work
	 */
	this->csv.newRow() << "computerTime";
	for (int i = 0; i < this->logicalInputChannels.size(); i++) {
		//std::string currCSVHeaderRep = this->logicalInputChannels[i]->getCSVHeaderRepresentation();
		//this->csv << currCSVHeaderRep;
		//this->csv.add(currCSVHeaderRep);
		if (!this->logicalInputChannels[i]->isLoggedToCSV())
		{
			continue; // skip this non-logged channel
		}
		if (!this->logicalInputChannels[i]->getReturnsContinuousValue())
		{
			// if this is not a continuous (analog-like) channel:
			auto currExpandedChannels = this->logicalInputChannels[i]->getExpandedFinalValuePortNames();
			for (auto curr_expanded_channel : currExpandedChannels)
			{
				this->csv << curr_expanded_channel; // Add the expanded channel to the CSV header
			}
		}
	}
	this->csv.writeToFile(this->fileFullPath, false);

	// Write the header to the analog .csv file:
	//FIXME: Do I need to implement functionality to the digital state ports for generality?
	this->csv_analog.newRow() << "computerTime";
	for (int i = 0; i < this->logicalInputChannels.size(); i++) {
		if (!this->logicalInputChannels[i]->isLoggedToCSV())
		{
			continue; // skip this non-logged channel
		}
		if (this->logicalInputChannels[i]->getReturnsContinuousValue())
		{
			// if this *is* a continuous (analog-like) channel:
			auto currExpandedChannels = this->logicalInputChannels[i]->getExpandedFinalValuePortNames();
			for (auto curr_expanded_channel : currExpandedChannels)
			{
				this->csv_analog << curr_expanded_channel; // Add the expanded channel to the CSV header
			} // end for expanded channel
		} // end if (continuous)
	} // end for
	this->csv_analog.writeToFile(this->fileFullPath_analog, false);

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
	this->scheduler = new Bosma::Scheduler(MAX_NUM_THREAD_PER_LABJACK);

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
	// TODO: see if the stream version needs to do anything special here
	// Probably need to do something with To stop stream, use LJM_eStreamStop.
	printf("Stopping stream...\n");
	this->ljStreamInfo.done = TRUE;
	this->err = LJM_eStreamStop(this->handle);
	ErrorCheck(this->err, "LJM_eStreamStop");

	this->ljStreamInfo.cleanup();
	
	//printf("Stream stopped. %u milliseconds have elapsed since LJM_eStreamStart\n", t1 - t0);

	// Tear down the ljStreamInfo struct, which had its arrays dynamically allocated:
	//free(this->ljStreamInfo.aScanList);
	//free(this->ljStreamInfo.aData);
	
	// Write out INI config if needed:
	//this->configMan->saveConfig();
	
	// Cleanup output ports vector
	for (int i = 0; i < NUM_OUTPUT_CHANNELS; i++) {
		delete this->outputPorts[i];
	}

	// Cleanup Input ports vector
	for (int i = 0; i < this->logicalInputChannels.size(); i++) {
		delete this->logicalInputChannels[i];
	}
	this->logicalInputChannels.clear();
	

	// Destroy the object's thread at the very start of its destructor
	delete this->scheduler;
	//this->scheduler = NULL;

	// Close the open output file:
	//this->outputFile.close();
	this->csv.writeToFile(this->fileFullPath, true);
	this->csv_analog.writeToFile(this->fileFullPath_analog, true);

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
	std::cout << ">> Labjack [" << this->serialNumber << "] :" << std::endl;
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
	return static_cast<time_t>(labjackTime);
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
					
					// Lock the mutex to prevent concurrent labjack interaction
					std::lock_guard<std::mutex> labjackLock(this->labjackMutex);
					this->err = LJM_eWriteName(this->handle, portName, outputValue);
					ErrorCheck(this->err, "LJM_eWriteName");
				}
				else {
					// a value greater than 0.0 means that the light should be off, so it should be set to input mode. This is accomplished by reading from the port (instead of writing).
					// Lock the mutex to prevent concurrent labjack interaction
					std::lock_guard<std::mutex> labjackLock(this->labjackMutex);
					double tempReadValue = 0.0;
					this->err = LJM_eReadName(this->handle, portName, &tempReadValue);
					ErrorCheck(this->err, "LJM_eReadName");
					//std::cout << "\t Visible LED Override: read from port. " << tempReadValue << std::endl;
				}
			}
			else {
				// Not the visible light relay and can be handled in the usual way.
				// Lock the mutex to prevent concurrent labjack interaction
				std::lock_guard<std::mutex> labjackLock(this->labjackMutex);
				this->err = LJM_eWriteName(this->handle, portName, outputValue);
				ErrorCheck(this->err, "LJM_eWriteName");
			}
			if (PRINT_OUTPUT_VALUES_TO_CONSOLE) {
				printf("\t Set %s state : %f\n", portName, outputValue);
			}
			
		} // end if didChange
	} // end for
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

int BehavioralBoxLabjack::getNumberInputChannels(bool include_digital_ports, bool include_analog_ports) {
	return this->getInputPortNames(include_digital_ports, include_analog_ports).size();
	//if (include_digital_ports && include_analog_ports)
	//{
	//	return NUM_CHANNELS;
	//}
	//if (include_digital_ports)
	//{
	//	return NUM_CHANNELS_DIGITAL;
	//}
	//if (include_analog_ports)
	//{
	//	return NUM_CHANNELS_ANALOG;
	//}
}

std::vector<std::string> BehavioralBoxLabjack::getInputPortNames(bool include_digital_ports, bool include_analog_ports)
{
	// Returns flat output strings
	std::vector<std::string> outputStrings = std::vector<std::string>();
	for (int i = 0; i < this->logicalInputChannels.size(); i++) {
		auto currChannel = this->logicalInputChannels[i];
		
		if (currChannel->getReturnsContinuousValue())
		{
			// It's analog:
			if (include_analog_ports)
			{
				for (auto output_string : currChannel->getPortNames())
				{
					outputStrings.push_back(output_string);
				}
			}
			else {
				continue;
			}
		}
		else {
			// It's digital:
			if (include_digital_ports)
			{
				for (auto output_string : currChannel->getPortNames())
				{
					outputStrings.push_back(output_string);
				}
			}
			else {
				continue;
			}
		}
	}
	//for (int i = 0; i < this->getNumberInputChannels(true, true); i++) {
	//	if (this->inputPortIsAnalog[i])
	//	{
	//		// It's analog:
	//		if (include_analog_ports)
	//		{
	//			currString = std::string(this->inputPortNames_all[i]);
	//			outputStrings.push_back(currString);
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else {
	//		// It's digital:
	//		if (include_digital_ports)
	//		{
	//			currString = std::string(this->inputPortNames_all[i]);
	//			outputStrings.push_back(currString);
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//}
	return outputStrings;
}

//std::vector<std::string> BehavioralBoxLabjack::getInputPortPurpose(bool include_digital_ports, bool include_analog_ports)
//{
//	std::vector<std::string> outputStrings = std::vector<std::string>();
//	std::string currString = "";
//	for (int i = 0; i < this->getNumberInputChannels(true, true); i++) {
//		if (this->inputPortIsAnalog[i])
//		{
//			// It's analog:
//			if (include_analog_ports)
//			{
//				currString = std::string(this->inputPortPurpose_all[i]);
//				outputStrings.push_back(currString);
//			}
//			else {
//				continue;
//			}
//		}
//		else {
//			// It's digital:
//			if (include_digital_ports)
//			{
//				currString = std::string(this->inputPortPurpose_all[i]);
//				outputStrings.push_back(currString);
//			}
//			else {
//				continue;
//			}
//		}
//	}
//	return outputStrings;
//}

std::vector<double> BehavioralBoxLabjack::getLastReadValues(bool include_digital_ports, bool include_analog_ports)
{
	std::vector<double> outputValues = std::vector<double>();
	for (int i = 0; i < this->getNumberInputChannels(true, true); i++) {
		if (this->inputPortIsAnalog[i])
		{
			// It's analog:
			if (include_analog_ports)
			{
				outputValues.push_back(this->lastReadInputPortValues[i]);
			}
			else {
				continue;
			}
		}
		else {
			// It's digital:
			if (include_digital_ports)
			{
				outputValues.push_back(this->lastReadInputPortValues[i]);
			}
			else {
				continue;
			}
		}
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
			std::cout << "\t Override<" << "Visible LED" << ">" << "Mode 0: Light Default Behavior" << std::endl;
		}
		else {
			// Otherwise if the LED is in (mode 1), transition to (mode 2)
			this->overrideValue_isVisibleLEDLit = true;
			this->isOverrideActive_VisibleLED = true;
			std::cout << "\t Override<" << "Visible LED" << ">" << "Mode 2: Light Forced ON" << std::endl;
		}
	}
	else {
		// Override mode isn't active (mode 0), transition to (mode 1)
		this->overrideValue_isVisibleLEDLit = false;
		this->isOverrideActive_VisibleLED = true;
		std::cout << "\t Override<" << "Visible LED" << ">" << "Mode 1: Light Forced OFF" << std::endl;
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
			std::cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 0: LEDs Default Behavior" << std::endl;
		}
		else {
			// Otherwise if the LED is in (mode 1), transition to (mode 2)
			this->overrideValue_areAttractModeLEDsLit = true;
			this->isOverrideActive_AttractModeLEDs = true;
			std::cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 2: LEDs Forced ON" << std::endl;
		}
	}
	else {
		// Override mode isn't active (mode 0), transition to (mode 1)
		this->overrideValue_areAttractModeLEDsLit = false;
		this->isOverrideActive_AttractModeLEDs = true;
		std::cout << "\t Override<" << "Port Attract LEDs" << ">" << "Mode 1: LEDs Forced OFF" << std::endl;
	}
}

bool BehavioralBoxLabjack::saveConfigurationFile(std::string filePath)
{
	// Saves the configuration INI out to file
	try
	{
		//TODO: Set path

		return this->configMan->saveConfig();
	}
	catch (...)
	{
		return false;
	}
}

//TODO: eventually to be replaced by dynamic loading from config file
void BehavioralBoxLabjack::testBuildLogicalInputChannels()
{
	// "AIN0", "AIN1", "AIN2", "AIN3"
	LabjackLogicalInputChannel* newInputChannel_A0 = new LabjackLogicalInputChannel({ "AIN0" }, { "Water1_BeamBreak" }, "AIN0");
	newInputChannel_A0->fn_generic_get_value = [](int numInputs, double* valuePointer)
	{
		auto currInputValue = valuePointer[0];
		// return a double
		if (LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(currInputValue))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	};
	
	this->logicalInputChannels.push_back(newInputChannel_A0);

	LabjackLogicalInputChannel* newInputChannel_A1 = new LabjackLogicalInputChannel({ "AIN1" }, { "Water2_BeamBreak" }, "AIN1");
	newInputChannel_A1->fn_generic_get_value = [](int numInputs, double* valuePointer)
	{
		auto currInputValue = valuePointer[0];
		// return a double
		if (LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(currInputValue))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	};
	this->logicalInputChannels.push_back(newInputChannel_A1);

	LabjackLogicalInputChannel* newInputChannel_A2 = new LabjackLogicalInputChannel({ "AIN2" }, { "Food1_BeamBreak" }, "AIN2");
	newInputChannel_A2->fn_generic_get_value = [](int numInputs, double* valuePointer)
	{
		auto currInputValue = valuePointer[0];
		// return a double
		if (LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(currInputValue))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	};
	this->logicalInputChannels.push_back(newInputChannel_A2);

	LabjackLogicalInputChannel* newInputChannel_A3 = new LabjackLogicalInputChannel({ "AIN3" }, { "Food2_BeamBreak" }, "AIN3");
	newInputChannel_A3->fn_generic_get_value = [](int numInputs, double* valuePointer)
	{
		auto currInputValue = valuePointer[0];
		// return a double
		if (LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(currInputValue))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	};
	this->logicalInputChannels.push_back(newInputChannel_A3);
	
	LabjackLogicalInputChannel* newInputChannel = new LabjackLogicalInputChannel({ "FIO_STATE" }, { "SIGNALS_Dispense" }, "SIGNALS_Dispense");
	this->logicalInputChannels.push_back(newInputChannel);
	//newInputChannel_A0->fn_generic_get_value = [](int numInputs, double* valuePointer)
	//{
	//	auto currInputValue = valuePointer[0];
	//	auto currBitsetValues = LabjackLogicalInputChannel::convertValue_DigitalStateAsDigitalValues(currInputValue);
	//	// return a double
	//	return currBitsetValues;
	//};
	

	LabjackLogicalInputChannel* timerInputChannel = new LabjackLogicalInputChannel({ "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16" }, { "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16" }, "Stream_Offset_Timer");
	timerInputChannel->loggingMode = LabjackLogicalInputChannel::FinalDesiredValueLoggingMode::NotLogged;
	timerInputChannel->setNumberOfDoubleInputs(2); // Takes 2 double values to produce its output
	timerInputChannel->fn_generic_get_value = [](int numInputs, double* valuePointer)
	{
		auto currInputValue_lowerBits = valuePointer[0];
		auto currInputValue_upperBits = valuePointer[1];

		// return a double
		auto stream_timer_value = LabjackLogicalInputChannel::convertValue_StreamTimer(currInputValue_upperBits, currInputValue_lowerBits);
		return double(stream_timer_value);
	};
	
	this->logicalInputChannels.push_back(timerInputChannel);

	
}

// Reads the device name and updates its value
std::string BehavioralBoxLabjack::readDeviceName()
{
	char nameString[LJM_STRING_ALLOCATION_SIZE];
	// Get device name
	this->err = LJM_eReadNameString(this->handle, "DEVICE_NAME_DEFAULT", nameString);
	if (this->err == LJME_NOERROR) {
		//printf("\t DEVICE_NAME_DEFAULT: %s\n", nameString);
		return std::string(nameString);
	}
	else
	{
		printf("\t This device does not have a name\n");
		return "";
	}
}

bool BehavioralBoxLabjack::writeDeviceName(std::string newDeviceName)
{
	if (newDeviceName.empty()) {
		return false;
	}
	const char* cNewNameString = newDeviceName.c_str();

	this->err = LJM_eWriteNameString(this->handle, "DEVICE_NAME_DEFAULT", cNewNameString);
	if (this->err == LJME_NOERROR) {
		// Verify the name has changed by reading it
		this->deviceName = this->readDeviceName();
		bool nameChangeSucceeded = (this->deviceName == newDeviceName);
		if (nameChangeSucceeded) {
			std::cout << "SUCCESS: Changed device name to " << newDeviceName << std::endl;
		}
		else {
			std::cout << "Failed to set device name to " << newDeviceName << std::endl;
			std::cout << "Current device name: " << this->deviceName << std::endl;
		}
		return nameChangeSucceeded;
	}
	else
	{
		std::cout << "Failed to set device name to " << newDeviceName << std::endl;
		this->deviceName = this->readDeviceName();
		std::cout << "Current device name: " << this->deviceName << std::endl;
		return false;
	}
}

// Configures Labjack's name and input/output port mappings if it hasn't already been done.
void BehavioralBoxLabjack::initializeLabjackConfigurationIfNeeded()
{
	// Get device name
	this->deviceName = this->readDeviceName();
	//printf("\t DEVICE_NAME_DEFAULT: %s\n", this->deviceName);

	// Make sure name is of the right format:
	bool needsRename = false;
	if (this->deviceName == "") {
		std::cout << "Hostname empty!" << std::endl;
		needsRename = true;
	}
	else {
		std::smatch stringMatch;    // same as std::match_results<string::const_iterator> sm;
		std::regex_match(this->deviceName, stringMatch, behavioral_box_labjack_deviceName_regex);
		if (stringMatch.size() <= 1) {
			std::cout << "Couldn't parse number from " << this->deviceName << ". It's not of the expected format \"LJ-XX\"." << std::endl;
			needsRename = true;
		}
		else {
			std::string numbersMatchString = stringMatch[1];
			int numberOutResult = std::stoi(numbersMatchString);
			std::cout << "Labjack has appropriate name: " << this->deviceName << std::endl;
			needsRename = false;
		}
	}
	// Prompt for the rename if needed:
	if (needsRename == true) {
		std::cout << "Labjack needs to be renamed..." << std::endl;
		std::ostringstream ostr;
		int computerIdentifierNumber = this->configMan->getNumericComputerIdentifier();
		if (computerIdentifierNumber == -1) {
			// Computer ID is bad.
			//TODO: prompt user for ID
			std::cout << "Please enter a new labjack ID number: ";
			std::cin >> computerIdentifierNumber;			
		}
		// Build new Labjack Name
		ostr << "LJ-";
		ostr << std::dec << std::setw(2) << std::setfill('0') << computerIdentifierNumber;
		std::string new_proposed_labjack_name_string = ostr.str(); //the str() function of the stream returns the string
		std::cout << "New Proposed Labjack Name: " << new_proposed_labjack_name_string << std::endl;

		std::cout << "Trying to change device name...." << std::endl;
		this->writeDeviceName(new_proposed_labjack_name_string);
		std::cout << "done." << std::endl;
	}

	// Labjack Stream Mode Setup:
	this->SetupStream();

}





void BehavioralBoxLabjack::SetupStream()
{
	const int STREAM_TRIGGER_INDEX = 0;
	const int STREAM_CLOCK_SOURCE = 0;
	const int STREAM_RESOLUTION_INDEX = 0;
	const double STREAM_SETTLING_US = 0;
	const double AIN_ALL_RANGE = 0;
	const int AIN_ALL_NEGATIVE_CH = LJM_GND;

	printf("Writing configurations:\n");

	// Tears down existing streams if they're already running:
	DisableStreamIfEnabled(this->handle);
	

	if (STREAM_TRIGGER_INDEX == 0) {
		printf("    Ensuring triggered stream is disabled:");
	}
	printf("    Setting STREAM_TRIGGER_INDEX to %d\n", STREAM_TRIGGER_INDEX);
	WriteNameOrDie(this->handle, "STREAM_TRIGGER_INDEX", STREAM_TRIGGER_INDEX);

	if (!EXTERNAL_STREAM_CLOCK) {
		printf("    Enabling internally-clocked stream:");
		printf("    Setting STREAM_CLOCK_SOURCE to %d\n", STREAM_CLOCK_SOURCE);
		WriteNameOrDie(this->handle, "STREAM_CLOCK_SOURCE", STREAM_CLOCK_SOURCE);
	}

	// Configure the analog inputs' negative channel, range, settling time and
	// resolution.
	// Note: when streaming, negative channels and ranges can be configured for
	// individual analog inputs, but the stream has only one settling time and
	// resolution.
	printf("    Setting STREAM_RESOLUTION_INDEX to %d\n",	STREAM_RESOLUTION_INDEX);
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
	printf("\n\n");
	WriteNameOrDie(this->handle, "AIN_ALL_NEGATIVE_CH", AIN_ALL_NEGATIVE_CH);

	// Build the stream object:
	//const char* CHANNEL_NAMES[] = globalLabjackInputPortNames;
	//auto channelNames = this->getInputPortNames();
	//const char** CHANNEL_NAMES = channelNames
	//const char* CHANNEL_NAMES[] = this->inputPortNames_all;
	
	//this->ljStreamInfo.build(this->getNumberInputChannels(true, true), CHANNEL_NAMES, 200);

	auto currChannelNames = this->getInputPortNames(true, true);
	
	
	//this->ljStreamInfo.build(this->getNumberInputChannels(true, true), CHANNEL_NAMES, 200);
	this->ljStreamInfo.build(currChannelNames, 200);

	this->err = LJM_NamesToAddresses(this->ljStreamInfo.numChannels, const_cast<const char**>(this->ljStreamInfo.channelNames), this->ljStreamInfo.aScanList, NULL);
	ErrorCheck(this->err, "Getting positive channel addresses");
	
	
	// Variables for LJM_eStreamStart
	this->err = LJM_eStreamStart(this->handle, this->ljStreamInfo.scansPerRead, this->ljStreamInfo.numChannels, this->ljStreamInfo.aScanList, &(this->ljStreamInfo.scanRate));
	ErrorCheck(this->err, "LJM_eStreamStart");

	//TODO: FIXME: We can't free the memory for aData yet, as it's needed in the next step, right?
	//free(this->ljStreamInfo.aScanList);
	//free(this->ljStreamInfo.aData);
	//TODO: should be freed after reading complete though, on shutdown.
	
}

bool BehavioralBoxLabjack::isArtificialDaylightHours()
{
	time_t currTime = time(nullptr);
	struct tm *currLocalTime = localtime(&currTime);

	int hour = currLocalTime->tm_hour;
	// Note this has been changed as of 8/16/2019.
	// globalDaylightStartHour: defines the hour of the day at which the Visible LEDS are turned on (illuminated) (simulating daylight for the mouse).
	// globalDaylightOffHour: defines the hour of the day at which the Visible LEDS are turned off (simulating nighttime for the mouse).
	if ((hour < configMan->getLoadedConfig().daylightStartHour) || (hour >= configMan->getLoadedConfig().daylightOffHour)) {
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




void BehavioralBoxLabjack::readSensorValues()
{
	this->lastCaptureComputerTime = Clock::now();
	static int streamRead = 0;

	unsigned int timeStart, timeEnd;


	// Check if stream is done so that we don't output the printf below
	if (this->ljStreamInfo.done) {
		return;
	}

	{ // make sure I"m not introducing a bug with my concurrency/mutex by having the variables be defined outside the block		
		// Lock the mutex to prevent concurrent labjack interaction
		std::lock_guard<std::mutex> labjackLock(this->labjackMutex);
		//printf("\niteration: % 3d    ", streamRead++);

		int deviceScanBacklog = 0;
		int LJMScanBacklog = 0;

		//timeStart = GetCurrentTimeMS();
		auto systemTimeStart = Clock::now();
		this->err = LJM_eStreamRead(this->handle, this->ljStreamInfo.aData, &deviceScanBacklog, &LJMScanBacklog);
		/*timeEnd = GetCurrentTimeMS();*/
		//auto systemTimeEnd = Clock::now();
		//printf("timerStart: %f\t timeEnd: %f\t difference: %f\n", double(timeStart), double(timeEnd), (double(timeEnd) - double(timeStart)));

		// If LJM has called this callback, the data is valid, but LJM_eStreamRead
		// may return LJME_STREAM_NOT_RUNNING if another thread has stopped stream,
		// such as this example program does in StreamWithCallback().
		if (this->err != LJME_NOERROR && this->err != LJME_STREAM_NOT_RUNNING) {
			PrintErrorIfError(this->err, "LJM_eStreamRead");

			// Tries to stop the stream:
			this->ljStreamInfo.done = 1;
			this->shouldStop = true;
			//this->err = LJM_eStreamStop(this->handle);
			//PrintErrorIfError(this->err, "LJM_eStreamStop");
			return;
		}

		// TODO: Assumes the last two channels are the timer channels:
		const int timer_lower_bits_index = this->ljStreamInfo.numChannels - 2;
		const int timer_upper_bits_index = this->ljStreamInfo.numChannels - 1;

		if (strcmp(this->inputPortNames_all[timer_lower_bits_index], "SYSTEM_TIMER_20HZ") != 0
			|| strcmp(this->inputPortNames_all[timer_upper_bits_index], "STREAM_DATA_CAPTURE_16") != 0)
		{
			//printf("%s:%d - BehavioralBoxLabjack::readSensorValues() - unexpected register: %s and/or %s\n", __FILE__, __LINE__, this->inputPortNames_all[timer_lower_bits_index], this->inputPortNames_all[timer_upper_bits_index]);
			this->ljStreamInfo.done = 1;
			this->shouldStop = true;
			return;
		}

		// Main:
		int scanStartOffsetI, chanI;
		int scanI = 0;

		int numSkippedScans = 0;
		//int maxScansPerChannel = limitScans ? MAX_NUM : numScans;
		unsigned short temp;
		unsigned char* bytes;

		unsigned short last_temp;
		unsigned char* last_bytes;

		double changeTolerance = 0.1; // The amount of change permitted without considering an event a change
		bool currDidChange = false;
		int memcmpDidChange = 0;

		// Goal is to find lines (scanI) where a value change occurs most efficiently
		bool currScanDidAnyChange = false;
		bool currScanDidAnyAnalogPortChange = false;
		bool currScanDidAnyDigitalPortChange = false;


		// Normally would allocate a double* buffer, right?
		double* lastReadValues = nullptr;
		lastReadValues = new double[this->ljStreamInfo.numChannels];

		double currScanTimeOffsetSinceFirstScan = this->ljStreamInfo.getTimeSinceFirstScan(1);

		unsigned int timerValue;
		unsigned int previousTimerValue;
		//unsigned int * timerValues = new unsigned int[this->ljStreamInfo.scansPerRead];

		// Otherwise it's good
		//printf("iteration: %d - deviceScanBacklog: %d, LJMScanBacklog: %d....\n", streamRead, deviceScanBacklog, LJMScanBacklog);

		scanStartOffsetI = 0;
		for (scanI = 0; scanI < this->ljStreamInfo.scansPerRead; scanI++) {
			currScanDidAnyChange = false;
			currScanDidAnyAnalogPortChange = false;
			currScanDidAnyDigitalPortChange = false;

			chanI = 0;
			for (int logicalChannelIndex = 0; logicalChannelIndex < this->logicalInputChannels.size(); logicalChannelIndex++) {
				auto currChannel = this->logicalInputChannels[logicalChannelIndex];
				auto currNumberOfDoublesToRead = currChannel->getNumberOfDoubleInputs();

				if (this->ljStreamInfo.aData[scanStartOffsetI + chanI] == LJM_DUMMY_VALUE) {
					++numSkippedScans;
					//FIXME: I think we need to handle this case if the scan is skipped, we shouldn't go on and use its values
					continue;;
				}

				double* updated_pointer = this->ljStreamInfo.aData + (scanStartOffsetI + chanI);
				auto curr_got_value = currChannel->fn_generic_get_value(currNumberOfDoublesToRead, updated_pointer);

				
				//switch (currNumberOfDoublesToRead)
				//{
				//case 1:
				//	auto updated_value = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
				//	//double* updated_pointer = this->ljStreamInfo.aData + (scanStartOffsetI + chanI);
				//	auto curr_got_value = currChannel->fn_generic_get_value(currNumberOfDoublesToRead, updated_pointer);
				//	
				//	break;
				//case 2:
				//	auto lower_bits = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
				//	auto upper_bits = this->ljStreamInfo.aData[scanStartOffsetI + chanI + 1];
				//		// arg0: upper bits are later
				//	auto updated_stream_timer = currChannel->fn_stream_timer(upper_bits, lower_bits);
				//	
				//	break;
				//default:
				//	// error
				//	break;
				//}


				lastReadValues[chanI] = curr_got_value;
				
				// Once done with this port, move the chanI (raw index into double* aray for current scan) to prepare for the next row
				chanI += currNumberOfDoublesToRead;
			}

			
			// Skip the two timer channels
			//for (chanI = 0; chanI < (this->ljStreamInfo.numChannels - 2); chanI++) {

			//	if (this->ljStreamInfo.aData[scanStartOffsetI + chanI] == LJM_DUMMY_VALUE) {
			//		++numSkippedScans;
			//		//FIXME: I think we need to handle this case if the scan is skipped, we shouldn't go on and use its values

			//	}

			//	// Otherwise, get the last read value and compare it to this value:
			//	if (this->inputPortIsAnalog[chanI]) {
			//		// aData[scanI + chanI] is a double
			//		if (scanI == 0)
			//		{
			//			// If it's the first scan for this channel channel, set the lastReadValue to the appropriate value:
			//			lastReadValues[chanI] = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
			//		}
			//		else
			//		{
			//			currDidChange = (fabs(this->ljStreamInfo.aData[scanStartOffsetI + chanI] - lastReadValues[chanI]) > changeTolerance);
			//			if (currDidChange)
			//			{
			//				currScanDidAnyAnalogPortChange = currScanDidAnyAnalogPortChange || true;
			//				currScanDidAnyChange = currScanDidAnyChange || true;
			//			}

			//			// Update the last read value either way:
			//			lastReadValues[chanI] = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
			//		}
			//	}
			//	else {
			//		temp = (unsigned short)this->ljStreamInfo.aData[scanStartOffsetI + chanI];
			//		bytes = (unsigned char*)&temp;

			//		if (scanI == 0)
			//		{
			//			// If it's the first scan for this channel channel, set the lastReadValue to the appropriate value:
			//			lastReadValues[chanI] = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
			//		}
			//		else
			//		{
			//			// Get last read values:
			//			last_temp = (unsigned short)lastReadValues[chanI];
			//			last_bytes = (unsigned char*)&last_temp;

			//			// it changed if any of the bytes of interest changed:
			//			//
			//			memcmpDidChange = memcmp(bytes, last_bytes, sizeof(unsigned char*));
			//			currDidChange = (memcmpDidChange != 0);
			//			if (currDidChange)
			//			{
			//				currScanDidAnyDigitalPortChange = currScanDidAnyDigitalPortChange || true;
			//				currScanDidAnyChange = currScanDidAnyChange || true;
			//			}

			//			// Update the last read value either way:
			//			lastReadValues[chanI] = this->ljStreamInfo.aData[scanStartOffsetI + chanI];
			//		}

			//	} // end else (if is analog)

			//} // end for chanI

			// Get timers:
			// Combine SYSTEM_TIMER_20HZ's lower 16 bits and STREAM_DATA_CAPTURE_16, which
			// contains SYSTEM_TIMER_20HZ's upper 16 bits
			timerValue = ((unsigned short)this->ljStreamInfo.aData[scanStartOffsetI + timer_upper_bits_index] << 16) +
				(unsigned short)this->ljStreamInfo.aData[scanStartOffsetI + timer_lower_bits_index];

			/*
			 * "Internal 32-bit system timer running at 1/2 core speed, thus normally 80M/2 => 40 MHz."
			 */

			 // Gets the timer value for this scanI (scan index), guessing this is MS
			if (currScanDidAnyChange)
			{
				currScanTimeOffsetSinceFirstScan = this->ljStreamInfo.getTimeSinceFirstScan(scanI);
				// Get the timerValue as a timepoint:
				//double currTimerOffsetSeconds = double(timerValue) * 40.0 * 1000000.0; // Convert to seconds
				double currTimerOffsetSeconds = double(timerValue); // Convert to seconds
				double timerDifference = double(previousTimerValue) - currTimerOffsetSeconds;

				// This value is in seconds, but we want whole values:
				long long int roundedMsValue = static_cast<long long int>(currScanTimeOffsetSinceFirstScan * 1000.0);
				auto estimatedScanTime = systemTimeStart + std::chrono::milliseconds(roundedMsValue);
				unsigned long long estimated_scan_milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(estimatedScanTime.time_since_epoch()).count();

				// Only persist the values if the state has changed.
				// Note: should ignore the last two entries in the array, since they're the timer and they'll always update
				if (this->monitor->refreshState(estimatedScanTime, lastReadValues)) {
					//TODO: should this be asynchronous? This would require passing in the capture time and read values
					//printf("refresh state returned true!");
					this->performPersistValues(estimated_scan_milliseconds_since_epoch, lastReadValues, currScanDidAnyAnalogPortChange, currScanDidAnyDigitalPortChange, true);
				}

			}

			previousTimerValue = timerValue; // copy the current timerValue to the previousTimerValue

			//scanI++; // update scanI
			scanStartOffsetI += this->ljStreamInfo.numChannels; // update scanStartOffsetI
		} // end for scanI

		// release the dynamically allocated memory:
		delete[] lastReadValues;
		lastReadValues = nullptr;

		streamRead++;
	}

}



// Reads the most recently read values and persists them to the available output modalities (file, TCP, etc) if they've changed or it's needed.
/*
 * this->lastCaptureComputerTime, inputPortValuesChanged, lastReadInputPortValues, previousReadInputPortValues_all, logMutex
 * inputPortIsAnalog, inputPortPurpose_all, water1PortEndIlluminationTime
 */
//

// New value that aims to be independent of the last values cached, thus allowing Stream mode persistance
void BehavioralBoxLabjack::performPersistValues(unsigned long long estimated_scan_milliseconds_since_epoch, double* lastReadValues, bool did_anyAnalogPortChange, bool did_anyDigitalPortChange, bool enableConsoleLogging)
{
	// Determine if the change occured in the analog ports, digital ports, or both
	CSVWriter newCSVLine_digitalOnly(",");
	CSVWriter newCSVLine_analogOnly(",");

	if (enableConsoleLogging) {
		this->printIdentifierLine();
		std::cout << "\t " << estimated_scan_milliseconds_since_epoch << ": ";
	}
	//newCSVLine.newRow() << milliseconds_since_epoch;
	newCSVLine_digitalOnly.newRow() << estimated_scan_milliseconds_since_epoch;
	newCSVLine_analogOnly.newRow() << estimated_scan_milliseconds_since_epoch;

	unsigned short temp;
	unsigned char* rawBytes;
	
	for (int i = 0; i < NUM_CHANNELS; i++) {
		auto currPortType = this->inputPortTypes_all[i];
		bool isOutput = this->inputPortIsLogged_all[i];

		if (isOutput) {
			switch (currPortType)
			{
			case LabjackPortType::Analog:
				// If it's an analog port:
				newCSVLine_analogOnly << lastReadValues[i];
				break;
			case LabjackPortType::Digital: 
				// Otherwise, it's a digital port
				newCSVLine_digitalOnly << lastReadValues[i];
				break;
			case LabjackPortType::DigitalState: 
				// Otherwise, it's a digital port, need to read all bitwise values that we're interested in
				//LJM_FLOAT32ToByteArray(lastReadValues[i],)

				auto test_vector = LabjackHelpers::parseDigitalStateChannelValue(lastReadValues[i]);

				temp = (unsigned short)lastReadValues[i];
				rawBytes = (unsigned char*)&temp;

				// Convert the unsigned char output to binary representation (https://stackoverflow.com/questions/8521638/exact-binary-representation-of-a-double)
				//The C++ standard does not guarantee 8-bit bytes
				unsigned char startMask = 1;
				while (0 != static_cast<unsigned char>(startMask << 1)) {
					startMask <<= 1;
				}
				bool hasLeadBit = false;   //set this to true if you want to see leading zeros
				size_t byteIndex;
				/*for (byteIndex = 0;byteIndex < sizeof(double); ++byteIndex) {*/
				//for (byteIndex = 0;byteIndex < sizeof(unsigned char); ++byteIndex) {
				for (byteIndex = 0;byteIndex < sizeof(unsigned short); ++byteIndex) {
					unsigned char bitMask = startMask;
					while (0 != bitMask) {
						if (0 != (bitMask & rawBytes[byteIndex])) {
							//std::cout << "1";
							newCSVLine_digitalOnly << 1;
							hasLeadBit = true;
						}
						else if (hasLeadBit) {
							newCSVLine_digitalOnly << 0;
							//std::cout << "0";
						}
						bitMask >>= 1;
					}
				}
				if (!hasLeadBit) {
					//std::cout << "0";
					newCSVLine_digitalOnly << 0;
				}
				//newCSVLine_digitalOnly << lastReadValues[i];
				break;
			}
	
			if (enableConsoleLogging) {
				std::cout << lastReadValues[i] << ", ";
			}
		}
	} //end for num channels
	if (enableConsoleLogging) {
		std::cout << std::endl;
	}
	// Lock the mutex to prevent concurrent persisting
	std::lock_guard<std::mutex> csvLock(this->logMutex);
	{
		//newCSVLine.writeToFile(fileFullPath, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
		if (did_anyAnalogPortChange)
		{
			// If an analog port changed, write out to the digital line
			newCSVLine_analogOnly.writeToFile(this->fileFullPath_analog, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
		}
		if (did_anyDigitalPortChange)
		{
			// If a digital port changed, write out to the digital line
			newCSVLine_digitalOnly.writeToFile(this->fileFullPath, true); //TODO: relies on CSV object's internal buffering and writes out to the file each time.
		}
	}

	
}