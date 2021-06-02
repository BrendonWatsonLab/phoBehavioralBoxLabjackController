#include "LabjackStreamDataDestination.h"

#include <LabJackM.h>

void LabjackStreamDataDestination::accumulateScans(int numScans, int numChannels, const char** channelNames, const int* channelAddresses, double* aData)
{

	int scanI, chanI;
	int numSkippedScans = 0;
	//int maxScansPerChannel = limitScans ? MAX_NUM : numScans;
	unsigned short temp;
	unsigned char* bytes;

	// Goal is to find lines (scanI) where a value change occurs most efficiently


	for (scanI = 0; scanI < numScans * numChannels; scanI += numChannels) {
		for (chanI = 0; chanI < numChannels; chanI++) {
			if (aData[scanI + chanI] == LJM_DUMMY_VALUE) {
				++numSkippedScans;
			}
			if (channelAddresses[chanI] < 1000) {
				//printf("aData[%3d]: %+.05f    ", scanI + chanI, aData[scanI + chanI]);
				// aData[scanI + chanI] is a double

			}
			else {
				temp = (unsigned short)aData[scanI + chanI];
				bytes = (unsigned char*)&temp;

				//printf("aData[%3d]: 0x ", scanI + chanI);
				//printf("%02x %02x", bytes[0], bytes[1]);
				//printf("  (% 7.00f)   ", aData[scanI + chanI]);
			}
		} // end for chanI
		//printf("\n");

	} // end for scanI
	
}
