# Refactoring to Stream documentation

## In src/BehavioralBoxLabjack.h
	// Main run loop
	void runPollingLoop();

	// Read Sensor values
	void readSensorValues();

### readSensorValues() defininition:
void BehavioralBoxLabjack::readSensorValues()
{
	this->lastCaptureComputerTime = Clock::now();

	//Read the sensor values from the labjack DIO and AIO Inputs
	{
		// Lock the mutex to prevent concurrent labjack interaction
		std::lock_guard<std::mutex> labjackLock(this->labjackMutex);
		this->err = LJM_eReadNames(this->handle, NUM_CHANNELS, (const char**)this->inputPortNames_all, this->lastReadInputPortValues, &this->errorAddress);
		ErrorCheckWithAddress(this->err, this->errorAddress, "readSensorValues - LJM_eReadNames");
	}

	// Only persist the values if the state has changed.
	if (this->monitor->refreshState(this->lastCaptureComputerTime, this->lastReadInputPortValues)) {
		//TODO: should this be asynchronous? This would require passing in the capture time and read values
		this->persistReadValues(true);
	}
}


### readSensorValues() is called from 2 places:

#### Also in the destructor:
	//Read the values and save them one more time, so we know when the end of data collection occured.
	this->readSensorValues();

	
#### The main run loop
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


### Stream Version will need:

#### Can read directly using readSensorValues() and LJM_eStreamRead.

#### In destructor:
To stop stream, use LJM_eStreamStop.