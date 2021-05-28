#pragma once
#include "BehavioralBoxLabjack.h"
#include <LabJackM.h>
//#include "External/C_C++_LJM/LJM_Utilities.h"
#include "External/C_C++_LJM/LJM_StreamUtilities.h" // Include the Stream utilities now

/* This file enables Labjack's Stream input/output mode

*/

// Labjack Streaming Helpers:
#define FALSE 0
#define TRUE 1

// Set to non-zero for external stream clock
#define EXTERNAL_STREAM_CLOCK FALSE

// Set FIO0 to pulse out. See EnableFIO0PulseOut()
#define FIO0_PULSE_OUT FALSE

//typedef struct StreamInfo {
//	int handle;
//	double scanRate;
//	int scansPerRead;
//
//	int streamLengthMS;
//	int done;
//
//	LJM_StreamReadCallback callback;
//
//	int numChannels;
//	int* aScanList;
//	const char** channelNames;
//
//	int aDataSize;
//	double* aData;
//
//	unsigned int numScansToPrint;
//} StreamInfo;


//class LabjackStreamInfo;




class LabjackStreamHelpers
{
public:
	LabjackStreamHelpers();
	~LabjackStreamHelpers();

	static void SetupStream(LabjackStreamInfo* si);

	/**
	 * Sets up stream using MyStreamReadCallback as the stream callback that LJM
	 * will call when stream data is ready.
	 * Para: si - a pointer to the given StreamInfo for stream
	**/
	//static void StreamWithCallback(StreamInfo* si);

	/**
	 * Prints scans of the channels:
	 *     "AIN0",  "FIO_STATE",  "SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16".
	 * Combines SYSTEM_TIMER_20HZ and STREAM_DATA_CAPTURE_16 to create the original
	 * 32-bit value of SYSTEM_TIMER_20HZ.
	**/
	static void HardcodedPrintScans(LabjackStreamInfo* si, int deviceScanBacklog, int LJMScanBacklog);



	///**
	// * The stream callback that LJM will call when stream data is ready.
	// * Para: arg - a pointer to the given StreamInfo for stream
	//**/
	//static void GlobalLabjackStreamReadCallback(void* arg);

};








