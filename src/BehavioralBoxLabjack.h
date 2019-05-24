/**
 * Name: BehavioralBoxLabjack.h
 * Desc: An object representing a single labjack used inside a behavioral box
**/

#include <LabJackM.h>
#include <time.h>


#pragma once
class BehavioralBoxLabjack
{
public:
	BehavioralBoxLabjack(int uniqueIdentifier, const char * devType, const char * connType, const char * iden);
	~BehavioralBoxLabjack();

	void diagnosticPrint();

	int getError();

	// Time
	time_t getTime();
	void setTime(time_t newTime);

	// Visible Light Relay Control
	void setVisibleLightRelayState(bool isOn);


private:
	int uniqueIdentifier;
	int portOrPipe, ipAddress, serialNumber, packetMaxBytes;
	int deviceType = LJM_dtANY;
	int connectionType = LJM_ctANY;
	int err;
	int handle;

	char string[LJM_STRING_ALLOCATION_SIZE];
	
};

