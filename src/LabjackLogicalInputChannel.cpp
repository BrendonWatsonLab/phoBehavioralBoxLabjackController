#include "LabjackLogicalInputChannel.h"

#include <cassert>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include "LabjackHelpers.h"


bool LabjackLogicalInputChannel::isLoggedToCSV()
{
	switch (this->loggingMode)
	{
	case FinalDesiredValueLoggingMode::LoggedOnlyToCSV: 
		return true;
	case FinalDesiredValueLoggingMode::LoggedToAll: 
		return true;
	default: 
		return false;
	}
}

bool LabjackLogicalInputChannel::isLoggedToConsole()
{
	switch (this->loggingMode)
	{
	case FinalDesiredValueLoggingMode::LoggedOnlyToConsole:
		return true;
	case FinalDesiredValueLoggingMode::LoggedToAll:
		return true;
	default:
		return false;
	}
}

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
	if (this->fn_get_expanded_port_names)
	{
		return this->fn_get_expanded_port_names(this);
	}
	// Otherwise do the default:

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

std::bitset<8> LabjackLogicalInputChannel::convertValue_DigitalStateAsDigitalValues(double doubleRepresentation)
{
	return LabjackHelpers::parseDigitalStateChannelValue(doubleRepresentation);
}

bool LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(double analogValue)
{
	return (analogValue >= 2.5); // if analogValue is greater than or equal to 2.5 (out of 5.0), return true indicating a HIGH signal
}

unsigned int LabjackLogicalInputChannel::convertValue_StreamTimer(double upper_bits, double lower_bits)
{
	// Combine SYSTEM_TIMER_20HZ's lower 16 bits and STREAM_DATA_CAPTURE_16, which
		// contains SYSTEM_TIMER_20HZ's upper 16 bits
	unsigned int timerValue = ((unsigned short)upper_bits << 16) +
		(unsigned short)lower_bits;
	return timerValue;
}

std::vector<double> LabjackLogicalInputChannel::toFinalDoublesVector(std::bitset<8> value)
{
	std::vector<double> output = std::vector<double>(8, 0.0);
	for (int i = 0; i < 8; i++)
	{
		if (value.test(i))
		{
			output[i] = 1.0;
		}
		else
		{
			output[i] = 0.0;
		}
	}
	return output;
}

std::function<std::vector<double>(int, double*)> LabjackLogicalInputChannel::getDefault_genericGetValueFcn_AnalogAsDigitalInput()
{
	auto fcn = [](int numInputs, double* valuePointer)
	{
		assert(numInputs == 1);
		auto currInputValue = valuePointer[0];
		// return a double
		if (LabjackLogicalInputChannel::convertValue_AnalogAsDigitalInput(currInputValue))
		{
			return std::vector<double>({ 1.0 });
		}
		else
		{
			return std::vector<double>({ 0.0 });
		}
	};
	return fcn;
}

std::function<std::vector<bool>(int, double*, double*)> LabjackLogicalInputChannel::getDefault_didChangeFcn_AnalogAsDigitalInput()
{
	auto fcn = [](int numInputs, double* oldValuePointer, double* newValuePointer)
	{
		assert(numInputs == 1);
		const double changeTolerance = 0.1;  // always use a tolerance when comparing floating points to avoid precision errors
		// The incomming values should already have been converted using convertValue_AnalogAsDigitalInput(...), so they both should be either 0.0 or 1.0.
		// This means that this tolerance isn't really necissary.

		auto prevInputValue = oldValuePointer[0];
		auto currInputValue = newValuePointer[0];

		bool currDidChange = (fabs(currInputValue - prevInputValue) > changeTolerance);
		return std::vector<bool>({ (currDidChange) });
	};
	return fcn;
}

std::function<std::vector<double>(int, double*)> LabjackLogicalInputChannel::getDefault_genericGetValueFcn_AnalogAsContinuousInput()
{
	auto fcn = [](int numInputs, double* valuePointer)
	{
		assert(numInputs == 1);
		auto currInputValue = valuePointer[0];
		// return a double
		return std::vector<double>({ currInputValue });
	};
	return fcn;
}

std::function<std::vector<bool>(int, double*, double*)> LabjackLogicalInputChannel::getDefault_didChangeFcn_AnalogAsContinuousInput()
{
	auto fcn = [](int numInputs, double* oldValuePointer, double* newValuePointer)
	{
		assert(numInputs == 1);
		const double changeTolerance = 0.1;

		auto prevInputValue = oldValuePointer[0];
		auto currInputValue = newValuePointer[0];

		bool currDidChange = (fabs(currInputValue - prevInputValue) > changeTolerance);
		return std::vector<bool>({ (currDidChange) });
	};
	return fcn;
}

// returns a vector<double> with one value for each expanded port value
std::function<std::vector<double>(int, double*)> LabjackLogicalInputChannel::getDefault_genericGetValueFcn_DigitalStateAsDigitalValues()
{
	auto fcn = [](int numInputs, double* valuePointer)
	{
		assert(numInputs == 1);
		
		auto currInputValue = valuePointer[0];
		auto currBitsetValues = LabjackLogicalInputChannel::convertValue_DigitalStateAsDigitalValues(currInputValue);
		// return a double vector
		return LabjackLogicalInputChannel::toFinalDoublesVector(currBitsetValues);
	};
	return fcn;
}

std::function<std::vector<bool>(int, double*, double*)> LabjackLogicalInputChannel::getDefault_didChangeFcn_DigitalStateAsDigitalValues()
{
	auto fcn = [](int numInputs, double* oldValuePointer, double* newValuePointer)
	{
		const double changeTolerance = 0.2; // always use a tolerance when comparing floating points to avoid precision errors
		std::vector<bool> output = std::vector<bool>(numInputs);
		for (int i = 0; i < numInputs; i++)
		{
			auto prevInputValue = oldValuePointer[i];
			auto currInputValue = newValuePointer[i];
			output[i] = (fabs(currInputValue - prevInputValue) > changeTolerance);		
		}
		return output;
	};
	return fcn;
}
