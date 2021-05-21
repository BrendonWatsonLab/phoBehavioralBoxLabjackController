#pragma once
#include "BehavioralBoxLabjack.h"
#include <vector>


class LabjackHelpers
{
public:
	LabjackHelpers();
	~LabjackHelpers();

	//static std::vector<BehavioralBoxLabjack*> activeLabjacks;

	static int TypeToNumRegs(int type);
	static int GetArrayTotal(const int * array, int arraySize);
	/**
 * Desc: Returns the byte offset of the currentIndex, given previous address types.
**/
	static int GetAddressBytesOffset(const int * aTypes, int currentIndex);
	static int GetDeviceBytesOffset(int deviceI, int totalNumBytes);
	//static void PrintQueryResults(int NumAddresses, const char ** aNames, const int * aAddresses, const int * aNumRegs, int totalNumBytes, unsigned char * aBytes, int deviceI);
	static void PrintQueryResults(int NumAddresses, const char ** aNames, const int * aAddresses, const int * aTypes, int totalNumBytes, unsigned char * aBytes, int deviceI);

	static std::vector<BehavioralBoxLabjack*> findAllLabjacks();
	static std::vector<BehavioralBoxLabjack*> findAllLabjacks(int previouslyFoundLabjackSerialNumbers[], int numPreviouslyFoundLabjacks);

	static bool blinkIsIlluminated();

	// Utility std::string and std::wstring conversion functions:
	static std::wstring s2ws(const std::string& str);
	static std::string ws2s(const std::wstring& wstr);

	// Windows Path manipulation functions:
	static std::string getCurrentWorkingDirectory();
	static std::string getFullPath(std::string relativePath);

	static bool showInExplorer(const std::string path);

	// Computes the change
	static std::vector<double> computeDelta(std::vector<double> V1, std::vector<double> V2);

	//TODO: Migrated to FormattingHelper.h: Redundant

	// Converts an unsigned long long value representing the milliseconds since epoch back to a time_point<Clock> (a datetime).
	static std::chrono::time_point<Clock> date_from_milliseconds_since_epoch(unsigned long long milliseconds_since_epoch);
	// Converts a time_point<Clock> (a datetime) to an unsigned long long value representing the milliseconds since epoch.
	static unsigned long long milliseconds_since_epoch_from_date(std::chrono::time_point<Clock> datetime);

};

