#include "LabjackLogicalInputChannel.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream


bool LabjackLogicalInputChannel::getReturnsContinuousValue()
{
	//TODO: Implement
    return false;
}

std::string LabjackLogicalInputChannel::getCSVHeaderRepresentation()
{
	auto portNames = this->getPortNames();
	if (portNames.size() == 1)
	{
		auto currPortName = portNames[0].c_str();
		
		std::stringstream ss;
		if (_strcmpi(currPortName, "FIO_STATE") == 0)
		{
			// There's 8 ports to add: FIO0 - FIO7
			ss << "FIO0" << "FIO1" << "FIO2" << "FIO3" << "FIO4" << "FIO5" << "FIO6" << "FIO7";
		}
		else if (_strcmpi(currPortName, "EIO_STATE") == 0)
		{
			// There's 8 ports to add: EIO0 - EIO7
			ss << "EIO0" << "EIO1" << "EIO2" << "EIO3" << "EIO4" << "EIO5" << "EIO6" << "EIO7";
		}
		else if (_strcmpi(currPortName, "MIO_STATE") == 0)
		{
			// There's 8 ports to add: MIO0 - MIO7
			ss << "MIO0" << "MIO1" << "MIO2" << "MIO3" << "MIO4" << "MIO5" << "MIO6" << "MIO7";
		}
		else if (_strcmpi(currPortName, "CIO_STATE") == 0)
		{
			// There's 8 ports to add: CIO0 - CIO7
			ss << "CIO0" << "CIO1" << "CIO2" << "CIO3" << "CIO4" << "CIO5" << "CIO6" << "CIO7";
		}
		else if (_strcmpi(currPortName, "DIO_STATE") == 0)
		{
			// There's 8 ports to add: DIO0 - DIO7
			ss << "DIO0" << "DIO1" << "DIO2" << "DIO3" << "DIO4" << "DIO5" << "DIO6" << "DIO7";
		}
		else
		{
			// Otherwise it's not a known digital state port, and is probably a regular digital port or an analog port
			ss << currPortName; // Just set the current port name as the string. 
		}
		return ss.str();
	}
	else
	{
		// portNames.size() is not 1
	}
	
    return std::string();
}
