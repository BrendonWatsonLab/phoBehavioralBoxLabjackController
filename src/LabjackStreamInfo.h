#pragma once
#include <LabJackM.h>

class LabjackStreamInfo
{
public:
	LabjackStreamInfo();
	//LabjackStreamInfo(std::string filePath);

	int handle;
	double scanRate;
	int scansPerRead;

	int streamLengthMS;
	
	LJM_StreamReadCallback callback;

	int numChannels;
	int* aScanList;
	const char** channelNames;

	int aDataSize;
	double* aData;

	unsigned int numScansToPrint;

	int done;

private:
	bool broken;
};

