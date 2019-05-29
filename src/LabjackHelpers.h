#pragma once
#include "BehavioralBoxLabjack.h"
#include <vector>

class LabjackHelpers
{
public:
	LabjackHelpers();
	~LabjackHelpers();

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

};

