#include "LabjackHelpers.h"
#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <string>
#include <iostream>
#include <LabJackM.h>
//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"

// For displaying the log file in Windows Explorer
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")

#include <locale>
#include <codecvt>
#define BUFSIZE 4096

#include <time.h>
#include <chrono>
typedef std::chrono::system_clock Clock;

LabjackHelpers::LabjackHelpers()
{
}


LabjackHelpers::~LabjackHelpers()
{
}

int LabjackHelpers::TypeToNumRegs(int type)
{
	switch (type) {
	case 0: // LJM_UINT16
		return 1;
	case 1: // LJM_UINT32
		return 2;
	case 2: // LJM_INT32
		return 2;
	case 3: // LJM_FLOAT32
		return 2;
	case 98: // LJM_STRING
		return 25;
	default:
		printf("%s:TypeToNumRegs:%d - Error: Type \'%d\' not recognized\n", __FILE__, __LINE__, type);
		return 0;
	}
}

int LabjackHelpers::GetArrayTotal(const int * array, int arraySize)
{
	int total = 0;
	int i;
	for (i = 0; i < arraySize; i++) {
		total += array[i];
	}

	return total;
}

int LabjackHelpers::GetAddressBytesOffset(const int * aTypes, int currentIndex)
{
	int addrI;
	int sum = 0;
	for (addrI = 0; addrI < currentIndex; addrI++) {
		sum += TypeToNumRegs(aTypes[addrI]);
	}
	return sum * LJM_BYTES_PER_REGISTER;
}

int LabjackHelpers::GetDeviceBytesOffset(int deviceI, int totalNumBytes)
{
	return deviceI * totalNumBytes;
}

void LabjackHelpers::PrintQueryResults(int NumAddresses, const char ** aNames, const int * aAddresses, const int * aTypes, int totalNumBytes, unsigned char * aBytes, int deviceI)
{
	int addrI, regI, byteI, endOfString;
	float float32;
	for (addrI = 0; addrI < NumAddresses; addrI++) {
		printf("            * %30s (address: % 6d): ", aNames[addrI], aAddresses[addrI]);
		if (aTypes[addrI] != LJM_STRING && aTypes[addrI] != LJM_FLOAT32) {
			printf("0x ");
		}

		endOfString = 0;
		for (regI = 0; regI < TypeToNumRegs(aTypes[addrI]); regI++) {
			byteI = GetDeviceBytesOffset(deviceI, totalNumBytes)
				+ GetAddressBytesOffset(aTypes, addrI)
				+ regI * LJM_BYTES_PER_REGISTER; // Bytes of current address offset

			if (aTypes[addrI] == LJM_STRING) {
				if (aBytes[byteI] == '\0') {
					endOfString = 1;
				}
				if (!endOfString) {
					printf("%c", aBytes[byteI]);
				}
				if (aBytes[byteI + 1] == '\0') {
					endOfString = 1;
				}
				if (!endOfString) {
					printf("%c", aBytes[byteI + 1]);
				}
			}
			else if (aTypes[addrI] == LJM_FLOAT32) {
				LJM_ByteArrayToFLOAT32(aBytes + byteI, 0, 1, &float32);
				printf("%0.04f", float32);
				break;
			}
			else {
				printf("%.02X %.02X ", aBytes[byteI], aBytes[byteI + 1]);
			}
		}
		printf("\n");
	}
}


std::vector<BehavioralBoxLabjack*> LabjackHelpers::findAllLabjacks()
{
	int emptyArray[LJM_LIST_ALL_SIZE] = {};
	return findAllLabjacks(emptyArray, 0);
}

std::vector<BehavioralBoxLabjack*> LabjackHelpers::findAllLabjacks(int previouslyFoundLabjackSerialNumbers[], int numPreviouslyFoundLabjacks)
{
	// Create a vector containing pointers (references) to Labjack Objects
	std::vector<BehavioralBoxLabjack*> outputVector = {};

	const int DeviceType = LJM_dtANY;
	const int ConnectionType = LJM_ctANY;
	enum { NumAddresses = 2 };
	const char * aNames[NumAddresses] = { "DEVICE_NAME_DEFAULT", "FIRMWARE_VERSION" };

	int err, i;
	int aAddresses[NumAddresses];
	int aTypes[NumAddresses];
	int aNumRegs[NumAddresses];
	int totalNumBytes;
	int maxNumFound = LJM_LIST_ALL_SIZE;
	unsigned char * aBytes = NULL;

	int aDeviceTypes[LJM_LIST_ALL_SIZE];
	int aConnectionTypes[LJM_LIST_ALL_SIZE];
	int aSerialNumbers[LJM_LIST_ALL_SIZE];
	int aIPAddresses[LJM_LIST_ALL_SIZE];
	int NumFound = 0;

	char IPv4String[LJM_IPv4_STRING_SIZE];

	err = LJM_NamesToAddresses(NumAddresses, aNames, aAddresses, aTypes);
	//ErrorCheck(err, "LJM_NamesToAddresses");

	for (i = 0; i < NumAddresses; i++) {
		aNumRegs[i] = LabjackHelpers::TypeToNumRegs(aTypes[i]);
	}
	totalNumBytes = LabjackHelpers::GetArrayTotal(aNumRegs, NumAddresses) * LJM_BYTES_PER_REGISTER;
	aBytes = (unsigned char *)malloc(sizeof(unsigned char) * totalNumBytes * maxNumFound);

	//printf("Calling LJM_ListAll with device type: %s, connection type: %s\n", NumberToDeviceType(DeviceType), NumberToConnectionType(ConnectionType));

	err = LJM_ListAllExtended(DeviceType, ConnectionType, NumAddresses, aAddresses, aNumRegs, maxNumFound, &NumFound, aDeviceTypes, aConnectionTypes, aSerialNumbers, aIPAddresses, aBytes);
	//ErrorCheck(err, "LJM_ListAllExtended with device type: %s, connection type: %s", NumberToDeviceType(DeviceType), NumberToConnectionType(ConnectionType));

	printf("Found %d device connections\n", NumFound);
	for (i = 0; i < NumFound; i++) {
		err = LJM_NumberToIP(aIPAddresses[i], IPv4String);
		//ErrorCheck(err, "LJM_NumberToIP");
		//printf("    [%3d] - aDeviceTypes: %s, aConnectionTypes: %s\n",i, NumberToDeviceType(aDeviceTypes[i]), NumberToConnectionType(aConnectionTypes[i]));
		//printf("            aSerialNumbers: %d, aIPAddresses: %s (%u)\n", aSerialNumbers[i], IPv4String, aIPAddresses[i]);
		//LabjackHelpers::PrintQueryResults(NumAddresses, aNames, aAddresses, aTypes, totalNumBytes, aBytes, i);
		bool wasLabjackAlreadyFound = false;
		if (numPreviouslyFoundLabjacks > 0) {
			// Check if the labjack was one that was previously found
			for (int j = 0; j < numPreviouslyFoundLabjacks; j++) {
				// Iterate through the existing arrays
				// Check if the serial numbers match
				if (aSerialNumbers[i] == previouslyFoundLabjackSerialNumbers[j]) {
					wasLabjackAlreadyFound = true;
					break;
				}
			}
			//TODO: detect if previously found labjacks have been removed.

		}

		if (!wasLabjackAlreadyFound) {
			// If the labjack found wasn't previously found (meaning that it's new), create a labjack object and add it to the output vector.
			BehavioralBoxLabjack* currLabjack = new BehavioralBoxLabjack(i, aDeviceTypes[i], aConnectionTypes[i], aSerialNumbers[i]);
			outputVector.push_back(currLabjack);
		}
	}

	free(aBytes);

	return outputVector;
}

bool LabjackHelpers::blinkIsIlluminated()
{
	time_t currTime = time(NULL);
	struct tm* currLocalTime = localtime(&currTime);
	int second = currLocalTime->tm_sec;
	// Return true if it's even. Changes every second
	return (second % 2);
}


std::wstring LabjackHelpers::s2ws(const std::string& str)
{
	// Converts std::string to std::wstring
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}


std::string LabjackHelpers::ws2s(const std::wstring& wstr)
{
	// Converts std::wstring to std::string
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

std::string LabjackHelpers::getCurrentWorkingDirectory()
{
	std::string outputString;
	DWORD retval = 0;
	TCHAR buffer[BUFSIZE] = TEXT("");
	retval = GetCurrentDirectory(BUFSIZE, buffer);

	if (retval == 0)
	{
		// Handle an error condition.
		printf("GetFullPathName failed (%d)\n", GetLastError());
		return std::string();
	}
	else
	{
		_tprintf(TEXT("The full path name is:  %s\n"), buffer);
		// From https://stackoverflow.com/questions/6291458/how-to-convert-a-tchar-array-to-stdstring (Aemmel)
		#ifndef UNICODE
			outputString = buffer;
		#else
			std::wstring wStr = buffer;
			outputString = std::string(wStr.begin(), wStr.end());
		#endif
		return outputString;
	}
	
	return outputString;
}

std::string LabjackHelpers::getFullPath(std::string relativePath)
{
	// Gets the full file path from the relative path
	std::string outputString;
	DWORD retval = 0;
	TCHAR buffer[BUFSIZE] = TEXT("");
	TCHAR** lppPart = { NULL };

	// From https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
	std::wstring stemp = s2ws(relativePath);
	LPCWSTR resultRelativePathString = stemp.c_str();
	// Retrieve the full path name for a file. 
	// The file does not need to exist.
	retval = GetFullPathName(resultRelativePathString,
		BUFSIZE,
		buffer,
		lppPart);

	if (retval == 0)
	{
		// Handle an error condition.
		printf("GetFullPathName failed (%d)\n", GetLastError());
		return std::string();
	}
	else
	{
		_tprintf(TEXT("The full path name is:  %s\n"), buffer);
		if (lppPart != NULL && *lppPart != 0)
		{
			_tprintf(TEXT("The final component in the path name is:  %s\n"), *lppPart);
		}

		// From https://stackoverflow.com/questions/6291458/how-to-convert-a-tchar-array-to-stdstring (Aemmel)
		#ifndef UNICODE
			outputString = buffer;
		#else
			std::wstring wStr = buffer;
			outputString = std::string(wStr.begin(), wStr.end());
		#endif
		return outputString;
	}

	return outputString;
}

bool LabjackHelpers::showInExplorer(const std::string path)
{
	//Displays the file at the path in its default application.
	ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
	return true;
}

std::vector<double> LabjackHelpers::computeDelta(std::vector<double> V1, std::vector<double> V2)
{
	std::vector<double> result;
	std::transform(V1.begin(), V1.end(), V2.begin(),
		std::back_inserter(result),
		std::minus<int>());
	return result;
}

//TODO: Migrated to FormattingHelper.h: Redundant
std::chrono::time_point<Clock> LabjackHelpers::date_from_milliseconds_since_epoch(unsigned long long milliseconds_since_epoch)
{
	std::chrono::milliseconds dur(milliseconds_since_epoch);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);
	return dt;
}

//TODO: Migrated to FormattingHelper.h: Redundant
unsigned long long LabjackHelpers::milliseconds_since_epoch_from_date(std::chrono::time_point<Clock> datetime)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(datetime.time_since_epoch()).count();
}


