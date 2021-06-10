#pragma once

#define MAX_CHANNEL_NAME_LENGTH 50

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
	//const char** channelNames = nullptr; // FIXME: these go out of scope

	//const char* channelNames[] = nullptr; // FIXME: these go out of scope
	char** channelNames = nullptr; 

	// 
	//int aDataSize = 0;
	unsigned int aDataSize;
	double* aData = nullptr;

	//unsigned int numScansToPrint = 1;
	int done = 0; // Done is 0 == FALSE

	void build(std::vector<std::string> channel_names, double scan_rate)
	{
		this->numChannels = channel_names.size();
		//this->channelNames = channel_names; // Old way that went out of scope

		// Allocate channel names:
		/*this->channelNames = new const char* [this->numChannels];*/
		this->channelNames = new char* [this->numChannels];

		for (int i = 0; i < this->numChannels; i++)
		{
			// Initialize to begin
			this->channelNames[i] = new char[MAX_CHANNEL_NAME_LENGTH];
			// Not necessary, but keeps things safe
			memset(this->channelNames[i], '\0', MAX_CHANNEL_NAME_LENGTH);
			// Copy the names to the string array
			//strncpy(this->channelNames[i], "DAC1", MAX_CHANNEL_NAME_LENGTH);
			strcpy_s(this->channelNames[i], MAX_CHANNEL_NAME_LENGTH, channel_names[i].c_str());
		}

		this->scanRate = scan_rate;
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
	
	//void build(int num_channels, const char** channel_names, double scan_rate)
	//{
	//	this->numChannels = num_channels;
	//	//this->channelNames = channel_names; // Old way that went out of scope

	//	// Allocate channel names:
	//	/*this->channelNames = new const char* [this->numChannels];*/
	//	this->channelNames = new char*[this->numChannels];
	//	
	//	for (int i = 0; i < num_channels; i++)
	//	{
	//		// Initialize to begin
	//		this->channelNames[i] = new char[MAX_CHANNEL_NAME_LENGTH];
	//		// Not necessary, but keeps things safe
	//		memset(this->channelNames[i], '\0', MAX_CHANNEL_NAME_LENGTH);
	//		// Copy the names to the string array
	//		//strncpy(this->channelNames[i], "DAC1", MAX_CHANNEL_NAME_LENGTH);
	//		strcpy_s(this->channelNames[i], MAX_CHANNEL_NAME_LENGTH, channel_names[i]);
	//	}
	//	
	//	this->scanRate = scan_rate;
	//	this->scansPerRead = this->scanRate / 2;

	//	this->aDataSize = this->numChannels * this->scansPerRead;
	//	// C++ style:
	//	//int* aScanList = new int[numChannels];
	//	//double* aData = new double[aDataSize];
	//	this->aScanList = new int[this->numChannels];
	//	this->aData = new double[aDataSize];

	//	// Clear aData. This is not strictly necessary, but can help debugging.
	//	memset(this->aData, 0, sizeof(double) * this->aDataSize);
	//}

	void cleanup()
	{
		delete[] this->aData;
		this->aData = nullptr;

		delete[] this->aScanList;
		this->aScanList = nullptr;

		delete[] this->channelNames;
		this->channelNames = nullptr;
	}


	// getTimeSinceFirstScan: computes the time since the first scan given the scanIndex (offset)
	double getTimeSinceFirstScan(int scanIndex)
	{
		//TimeSinceFirstScan = Offset * (1 / ScanRate);
		return (double(scanIndex) * (1.0 / this->scanRate));
	}

	// Extended functions:
	

private:
	bool broken = false;
};

