#pragma once

struct LabjackStreamInfo
{
public:
	//LabjackStreamInfo();
	//LabjackStreamInfo(std::string filePath);

	int handle = 0;
	double scanRate = 2000; // Set the scan rate to the fastest rate expected
	int scansPerRead = 1000; // Should usually be (this.scanRate / 2)

	int streamLengthMS = 10000;
	
	//LJM_StreamReadCallback callback;

	int numChannels = 0;
	int* aScanList = nullptr;
	const char** channelNames = nullptr;

	int aDataSize = 0;
	double* aData = nullptr;

	unsigned int numScansToPrint = 1;
	int done = 0; // Done is 0 == FALSE

	void setup(int numChannels, int handle)
	{
		this->numChannels = numChannels;
		this->scansPerRead = this.scanRate / 2;
	}
	

private:
	bool broken = false;
};

