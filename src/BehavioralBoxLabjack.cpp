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

#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"


BehavioralBoxLabjack::BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden)
{
	this->uniqueIdentifier = uniqueIdentifier;
	this->err = LJM_OpenS(devType, connType, iden, &handle);
	ErrorCheck(this->err, "LJM_OpenS");

	// Get device name
	this->err = LJM_eReadNameString(this->handle, "DEVICE_NAME_DEFAULT", this->string);
	if (this->err == LJME_NOERROR)
		printf("DEVICE_NAME_DEFAULT: %s\n", this->string);
	else
		printf("This device does not have a name\n");

	// Get device info
	this->err = LJM_GetHandleInfo(handle, &deviceType, &connectionType, &serialNumber, &ipAddress,
		&portOrPipe, &packetMaxBytes);
	ErrorCheck(this->err, "LJM_GetHandleInfo");

	this->diagnosticPrint();
	
}


BehavioralBoxLabjack::~BehavioralBoxLabjack()
{
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

