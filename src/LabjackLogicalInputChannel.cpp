#include "LabjackLogicalInputChannel.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream


bool LabjackLogicalInputChannel::getReturnsContinuousValue()
{
	//TODO: Implement
	switch (this->finalValueType)
	{
	case InputFinalDesiredValueType::Discrete: 
		return false;
		break;
	case InputFinalDesiredValueType::Continuous: 
		return true;
		break;
	}
	return false;
}

std::string LabjackLogicalInputChannel::getCSVHeaderRepresentation()
{
	auto portNames = this->getPortNames();
	if (portNames.size() == 1)
	{
		auto currPortName = portNames[0].c_str();
		
		std::stringstream ss;
		std::string delimiter = ",";
		if (_strcmpi(currPortName, "FIO_STATE") == 0)
		{
			// There's 8 ports to add: FIO0 - FIO7
			ss << "FIO0" << delimiter << "FIO1" << delimiter << "FIO2" << delimiter << "FIO3" << delimiter << "FIO4" << delimiter << "FIO5" << delimiter << "FIO6" << delimiter << "FIO7";
		}
		else if (_strcmpi(currPortName, "EIO_STATE") == 0)
		{
			// There's 8 ports to add: EIO0 - EIO7
			ss << "EIO0" << delimiter << "EIO1" << delimiter << "EIO2" << delimiter << "EIO3" << delimiter << "EIO4" << delimiter << "EIO5" << delimiter << "EIO6" << delimiter << "EIO7";
		}
		else if (_strcmpi(currPortName, "MIO_STATE") == 0)
		{
			// There's 8 ports to add: MIO0 - MIO7
			ss << "MIO0" << delimiter << "MIO1" << delimiter << "MIO2" << delimiter << "MIO3" << delimiter << "MIO4" << delimiter << "MIO5" << delimiter << "MIO6" << delimiter << "MIO7";
		}
		else if (_strcmpi(currPortName, "CIO_STATE") == 0)
		{
			// There's 8 ports to add: CIO0 - CIO7
			ss << "CIO0" << delimiter << "CIO1" << delimiter << "CIO2" << delimiter << "CIO3" << delimiter << "CIO4" << delimiter << "CIO5" << delimiter << "CIO6" << delimiter << "CIO7";
		}
		else if (_strcmpi(currPortName, "DIO_STATE") == 0)
		{
			// There's 8 ports to add: DIO0 - DIO7
			ss << "DIO0" << delimiter << "DIO1" << delimiter << "DIO2" << delimiter << "DIO3" << delimiter << "DIO4" << delimiter << "DIO5" << delimiter << "DIO6" << delimiter << "DIO7";
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

std::vector<std::string> LabjackLogicalInputChannel::getExpandedFinalValuePortNames()
{
	auto portNames = this->getPortNames();
	if (portNames.size() == 1)
	{
		auto currPortName = portNames[0].c_str();
		if (_strcmpi(currPortName, "FIO_STATE") == 0)
		{
			// There's 8 ports to add: FIO0 - FIO7
			return { "FIO0","FIO1","FIO2","FIO3","FIO4","FIO5","FIO6","FIO7" };

		}
		else if (_strcmpi(currPortName, "EIO_STATE") == 0)
		{
			// There's 8 ports to add: EIO0 - EIO7
			return { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7" };
		}
		else if (_strcmpi(currPortName, "MIO_STATE") == 0)
		{
			// There's 8 ports to add: MIO0 - MIO7
			return { "MIO0","MIO1","MIO2","MIO3","MIO4","MIO5","MIO6","MIO7" };
		}
		else if (_strcmpi(currPortName, "CIO_STATE") == 0)
		{
			// There's 8 ports to add: CIO0 - CIO7
			return { "CIO0","CIO1","CIO2","CIO3","CIO4","CIO5","CIO6","CIO7" };
		}
		else if (_strcmpi(currPortName, "DIO_STATE") == 0)
		{
			// There's 8 ports to add: DIO0 - DIO7
			return {"DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7"};
		}
		else
		{
			// Otherwise it's not a known digital state port, and is probably a regular digital port or an analog port
			return { currPortName }; // Just set the current port name as the string. 
		}
	}
	else
	{
		// portNames.size() is not 1
		return portNames;
	}

	return std::vector<std::string>();
}
