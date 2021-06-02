/**
 * Name: stream_basic.c
 * Desc: Demonstrates the most basic usage of stream using the LJM eStream
 *       functions.
**/

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */


#include <stdio.h>
#include <string.h>

#include <LabJackM.h>
#include <stdbool.h>

#include "../../LJM_StreamUtilities.h"


// Pho Custom:
#include "LabjackStreamDataDestination.h"


void Stream(int handle, int numChannels, const char ** channelNames, double scanRate, int scansPerRead, int numReads);

void HardcodedConfigureStream(int handle);


void PhoAccumulateScans(int numScans, int numChannels, double* aData, int* numSkippedSamples);


const boolean isAnalogChannel[] = { true, true, true, true, false };


int main()
{
	int handle;

	// How fast to stream in Hz
	//double INIT_SCAN_RATE = 2000;
	double INIT_SCAN_RATE = 200;

	// How many scans to get per call to LJM_eStreamRead. INIT_SCAN_RATE/2 is
	// recommended
	int SCANS_PER_READ = INIT_SCAN_RATE / 2;

	// How many times to call LJM_eStreamRead before calling LJM_eStreamStop
	const int NUM_READS = 10;

	// Channels/Addresses to stream. NUM_CHANNELS can be less than or equal to
	// the size of CHANNEL_NAMES
	//enum { NUM_CHANNELS = 2 };
	//const char * CHANNEL_NAMES[] = {"AIN0", "AIN1"};

	
	//enum { NUM_CHANNELS = 8 };
	//const char * CHANNEL_NAMES[] = {"AIN0", "AIN1", "AIN2", "AIN3", "FIO0", "FIO1", "FIO2", "FIO3"};

	//enum { NUM_CHANNELS = 4 };
	//const char* CHANNEL_NAMES[] = {"AIN0", "AIN1", "AIN2", "AIN3"};

	enum { NUM_CHANNELS = 5 };
	const char* CHANNEL_NAMES[] = { "AIN0", "AIN1", "AIN2", "AIN3", "FIO_STATE" };
	

	// FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
	// FIO_EIO_STATE: Read the state of the 16 bits of FIO-EIO in a single binary encoded value. 0=Low AND 1=HIGH
	
	// Open first found LabJack
	handle = OpenOrDie(LJM_dtANY, LJM_ctANY, "LJM_idANY");
	// handle = OpenSOrDie("LJM_dtANY", "LJM_ctANY", "LJM_idANY");

	PrintDeviceInfoFromHandle(handle);
	printf("\n");

	Stream(handle, NUM_CHANNELS, CHANNEL_NAMES, INIT_SCAN_RATE, SCANS_PER_READ, NUM_READS);

	CloseOrDie(handle);

	WaitForUserIfWindows();

	return LJME_NOERROR;
}

void HardcodedConfigureStream(int handle)
{
	const int STREAM_TRIGGER_INDEX = 0;
	const int STREAM_CLOCK_SOURCE = 0;
	const int STREAM_RESOLUTION_INDEX = 0;
	const double STREAM_SETTLING_US = 0;
	const double AIN_ALL_RANGE = 0;
	const int AIN_ALL_NEGATIVE_CH = LJM_GND;

	printf("Writing configurations:\n");

	if (STREAM_TRIGGER_INDEX == 0) {
		printf("    Ensuring triggered stream is disabled:");
	}
	printf("    Setting STREAM_TRIGGER_INDEX to %d\n", STREAM_TRIGGER_INDEX);
	WriteNameOrDie(handle, "STREAM_TRIGGER_INDEX", STREAM_TRIGGER_INDEX);

	if (STREAM_CLOCK_SOURCE == 0) {
		printf("    Enabling internally-clocked stream:");
	}
	printf("    Setting STREAM_CLOCK_SOURCE to %d\n", STREAM_CLOCK_SOURCE);
	WriteNameOrDie(handle, "STREAM_CLOCK_SOURCE", STREAM_CLOCK_SOURCE);

	// Configure the analog inputs' negative channel, range, settling time and
	// resolution.
	// Note: when streaming, negative channels and ranges can be configured for
	// individual analog inputs, but the stream has only one settling time and
	// resolution.
	printf("    Setting STREAM_RESOLUTION_INDEX to %d\n",
		STREAM_RESOLUTION_INDEX);
	WriteNameOrDie(handle, "STREAM_RESOLUTION_INDEX", STREAM_RESOLUTION_INDEX);

	printf("    Setting STREAM_SETTLING_US to %f\n", STREAM_SETTLING_US);
	WriteNameOrDie(handle, "STREAM_SETTLING_US", STREAM_SETTLING_US);

	printf("    Setting AIN_ALL_RANGE to %f\n", AIN_ALL_RANGE);
	WriteNameOrDie(handle, "AIN_ALL_RANGE", AIN_ALL_RANGE);

	printf("    Setting AIN_ALL_NEGATIVE_CH to ");
	if (AIN_ALL_NEGATIVE_CH == LJM_GND) {
		printf("LJM_GND");
	}
	else {
		printf("%d", AIN_ALL_NEGATIVE_CH);
	}
	printf("\n");
	WriteNameOrDie(handle, "AIN_ALL_NEGATIVE_CH", AIN_ALL_NEGATIVE_CH);
}

void PhoAccumulateScans(int numScans, int numChannels, double* aData, int* numSkippedSamples)
{
	int scanStartOffsetI, chanI;
	int scanI = 0;
	numSkippedSamples = 0;
	
	int numSkippedScans = 0;
	//int maxScansPerChannel = limitScans ? MAX_NUM : numScans;
	unsigned short temp;
	unsigned char* bytes;

	double changeTolerance = 1.0; // The amount of change permitted without considering an event a change
	bool currDidChange = false;

	// Goal is to find lines (scanI) where a value change occurs most efficiently

	// Normally would allocate a double* buffer, right?
	double* lastReadValues = nullptr;
	lastReadValues = new double[numChannels];
	
	for (scanStartOffsetI = 0; scanStartOffsetI < numScans * numChannels; scanStartOffsetI += numChannels) {
		for (chanI = 0; chanI < numChannels; chanI++) {

			if (aData[scanStartOffsetI + chanI] == LJM_DUMMY_VALUE) {
				++numSkippedScans;
				++numSkippedSamples;
			}
			
			if (scanI == 0)
			{
				// If it's the first scan for this channel channel, set the lastReadValue to the appropriate value:
				lastReadValues[chanI] = aData[scanStartOffsetI + chanI];
			}
			else
			{
				// Otherwise, get the last read value and compare it to this value:
				//if (channelAddresses[chanI] < 1000) {
				//	//printf("aData[%3d]: %+.05f    ", scanI + chanI, aData[scanI + chanI]);
				//	// aData[scanI + chanI] is a double

				//}
				//else {
				//	temp = (unsigned short)aData[scanI + chanI];
				//	bytes = (unsigned char*)&temp;

				//	//printf("aData[%3d]: 0x ", scanI + chanI);
				//	//printf("%02x %02x", bytes[0], bytes[1]);
				//	//printf("  (% 7.00f)   ", aData[scanI + chanI]);
				//}


				//bool didChange = (lastReadValues[chanI] == aData[scanI + chanI]);
				currDidChange = ((aData[scanStartOffsetI + chanI] - lastReadValues[chanI]) > changeTolerance);
				
				if (currDidChange)
				{
					//printf("didChange: aData[%3d]: %+.05f    \n", scanStartOffsetI + chanI, aData[scanStartOffsetI + chanI]);
					printf("didChange: aData[%3d, %3d]: %+.05f    \n", scanI, chanI, aData[scanStartOffsetI + chanI]);					
				}

				// Update the last read value either way:
				lastReadValues[chanI] = aData[scanStartOffsetI + chanI];
			}

			
		} // end for chanI
		//printf("\n");

		scanI++; // update scanI
	} // end for scanI

	// release the dynamically allocated memory:
	delete[] lastReadValues;
	lastReadValues = NULL;
	
}

void Stream(int handle, int numChannels, const char ** channelNames, double scanRate, int scansPerRead, int numReads)
{
	int err, iteration, channel;
	int numSkippedScans = 0;
	int totalSkippedScans = 0;
	int deviceScanBacklog = 0;
	int LJMScanBacklog = 0;
	unsigned int receiveBufferBytesSize = 0;
	unsigned int receiveBufferBytesBacklog = 0;
	int connectionType;

	char* formatString;
	unsigned short temp;
	unsigned char* bytes;

	unsigned int aDataSize = numChannels * scansPerRead;

	//// C style:
	//int* aScanList = malloc(sizeof(int) * numChannels);
	//double * aData = malloc(sizeof(double) * aDataSize);

	// C++ style:
	int* aScanList = new int[numChannels];
	double* aData = new double[aDataSize];

	err = LJM_GetHandleInfo(handle, NULL, &connectionType, NULL, NULL, NULL,	NULL);
	ErrorCheck(err, "LJM_GetHandleInfo");

	// Clear aData. This is not strictly necessary, but can help debugging.
	memset(aData, 0, sizeof(double) * aDataSize);

	err = LJM_NamesToAddresses(numChannels, channelNames, aScanList, NULL);
	ErrorCheck(err, "Getting positive channel addresses");

	HardcodedConfigureStream(handle);

	printf("\n");
	printf("Starting stream...\n");
	err = LJM_eStreamStart(handle, scansPerRead, numChannels, aScanList,
		&scanRate);
	ErrorCheck(err, "LJM_eStreamStart");
	printf("Stream started. Actual scan rate: %.02f Hz (%.02f sample rate)\n",
		scanRate, scanRate * numChannels);
	printf("\n");

	// Read the scans
	printf("Now performing %d reads\n", numReads);
	printf("\n");
	for (iteration = 0; iteration < numReads; iteration++) {
		// reads SCANS_PER_READ for each iteration of this loop
		err = LJM_eStreamRead(handle, aData, &deviceScanBacklog, &LJMScanBacklog);
		ErrorCheck(err, "LJM_eStreamRead");

		printf("iteration: %d - deviceScanBacklog: %d, LJMScanBacklog: %d", iteration, deviceScanBacklog, LJMScanBacklog);
		if (connectionType != LJM_ctUSB) {
			err = LJM_GetStreamTCPReceiveBufferStatus(handle,
				&receiveBufferBytesSize, &receiveBufferBytesBacklog);
			ErrorCheck(err, "LJM_GetStreamTCPReceiveBufferStatus");
			printf(", receive backlog: %f%%",
				((double)receiveBufferBytesBacklog) / receiveBufferBytesSize * 100);
		}
		printf("\n");

		numSkippedScans = 0;
		PhoAccumulateScans(scansPerRead, numChannels, aData, &numSkippedScans);

		//printf("  1st scan out of %d:\n", scansPerRead);

		//

		//
		//for (channel = 0; channel < numChannels; channel++) {
		//	if (isAnalogChannel[channel])
		//	{
		//		printf("    %s = %0.5f\n", channelNames[channel], aData[channel]);
		//	}
		//	else
		//	{
		//		//double currValue = aData[channel];

		//		
		//		/*UINT16 readDigitalValues = static_cast<UINT16>(aData[channel]);*/
		//		//auto bitcastValue = std::bit_cast<int>(aData[channel]);

		//		//UINT16 readDigitalValues = static_cast<UINT16>(aData[channel]);
		//		//UINT16 readDigitalValues = *(UINT16*)&currValue;
		//		//int readDigitalValues = *(int*)&currValue;

		//		temp = (unsigned short)aData[channel];
		//		bytes = (unsigned char*)&temp;
		//		
		//		printf("aData[%3d]: 0x ", channel);
		//		printf("%02x %02x", bytes[0], bytes[1]);
		//		printf("  (% 7.00f)   \n", aData[channel]);
		//		
		//		//printf("    %s = %hu\n", channelNames[channel], readDigitalValues);
		//	
		//	}
		//}
		
		//numSkippedScans = CountAndOutputNumSkippedSamples(numChannels, scansPerRead, aData);

		if (numSkippedScans) {
			printf("  %d skipped scans in this LJM_eStreamRead\n", numSkippedScans);
			totalSkippedScans += numSkippedScans;
		}
		printf("\n");

		
	}
	if (totalSkippedScans) {
		printf("\n****** Total number of skipped scans: %d ******\n\n", totalSkippedScans);
	}

	printf("Stopping stream\n");
	err = LJM_eStreamStop(handle);
	ErrorCheck(err, "Stopping stream");

	//// C style:
	//free(aData);
	//free(aScanList);

	// C++ Style:
	delete[] aData;
	aData = NULL;
	delete[] aScanList;
	aScanList = NULL;
}
