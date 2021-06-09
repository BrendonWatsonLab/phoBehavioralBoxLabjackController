
 
BehavioralBoxLabjack::initializeLabjackConfigurationIfNeeded()
	> BehavioralBoxLabjack::SetupStream()
		> Calls LJM_eStreamStart(...) with the labjack addresses to be included in the stream


## Primary Operation:
The main run loop is BehavioralBoxLabjack::runPollingLoop() and is called at a fixed frequency (in Hz).
	> BehavioralBoxLabjack::readSensorValues()
		> LJM_eStreamRead(...): Gets the values for the addresses requested in LJM_eStreamStart(...) and saves them into this->ljStreamInfo.aData as an array of double values (double *), one for each address


## Important Notes and Gotchas:

Note: const char** x & const char* x[] are same. The 2nd syntax is helpful when one wants to "represent" an array.
const char ** aNames, const int * aAddresse

The primary goal is to use modern C++ best-practices for data types while allowing compatibility with the Labjack Stream functions, which ask for C-style variables as inputs:

For example, 
	LJM_ERROR_RETURN LJM_eStreamStart(int Handle, int ScansPerRead, int NumAddresses, const int* aScanList, double* ScanRate);
	LJM_ERROR_RETURN LJM_eStreamRead(int Handle, double* aData, int* DeviceScanBacklog, int* LJMScanBacklog);


Want to be able to access `const int*` and `double*` type variables

C-style: `(const char **)variable`
C++-style: `const_cast<const char**>(variable)`

