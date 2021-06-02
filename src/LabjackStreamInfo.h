#pragma once

struct LabjackStreamInfo
{
public:
	//LabjackStreamInfo();
	//LabjackStreamInfo(std::string filePath);

	//~LabjackStreamInfo();
	
	//int handle = 0;
	double scanRate = 2000; // Set the scan rate to the fastest rate expected
	int scansPerRead = 1000; // Should usually be (this.scanRate / 2)

	//int streamLengthMS = 10000;	
	//LJM_StreamReadCallback callback;

	int numChannels = 0;
	int* aScanList = nullptr;
	const char** channelNames = nullptr;

	//int aDataSize = 0;
	unsigned int aDataSize;
	double* aData = nullptr;

	//unsigned int numScansToPrint = 1;
	int done = 0; // Done is 0 == FALSE

	void build(int numChannels, const char** channelNames, double scanRate)
	{
		this->numChannels = numChannels;
		this->channelNames = channelNames;
		
		this->scanRate = scanRate;
		this->scansPerRead = this->scanRate / 2;

		this->aDataSize = this->numChannels * this->scansPerRead;
		// C++ style:
		//int* aScanList = new int[numChannels];
		//double* aData = new double[aDataSize];
		this->aScanList = new int[this->numChannels];
		this->aData = new double[aDataSize];

		// Clear aData. This is not strictly necessary, but can help debugging.
		memset(this->aData, 0, sizeof(double) * this->aDataSize);
		
	}

	void cleanup()
	{
		delete[] this->aData;
		this->aData = nullptr;

		delete[] this->aScanList;
		this->aScanList = nullptr;
	}


	// getTimeSinceFirstScan: computes the time since the first scan given the scanIndex (offset)
	double getTimeSinceFirstScan(int scanIndex)
	{
		//TimeSinceFirstScan = Offset * (1 / ScanRate);
		return (double(scanIndex) * (1.0 / this->scanRate));
	}

private:
	bool broken = false;
};

