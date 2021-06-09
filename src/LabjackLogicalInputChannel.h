#pragma once
#include <bitset>

#include "LabjackLogicalChannelBase.h"

class LabjackLogicalInputChannel : public LabjackLogicalChannelBase
{
public:

	enum class InputFinalDesiredValueType { Discrete, Continuous };
	enum class FinalDesiredValueLoggingMode { NotLogged, LoggedOnlyToConsole, LoggedOnlyToCSV, LoggedToAll };

	InputFinalDesiredValueType finalValueType;

	FinalDesiredValueLoggingMode loggingMode;
	bool isLoggedToCSV();
	bool isLoggedToConsole();
	
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name, const LabjackLogicalInputChannel::InputFinalDesiredValueType& value_type, const FinalDesiredValueLoggingMode& logging_mode,
		const size_t& doubleInputs)
		: LabjackLogicalChannelBase(port_names, port_purpose, name, LabjackLogicalChannelBase::Direction::Input),
		  finalValueType(value_type), loggingMode(logging_mode), numDoubleInputValues(doubleInputs)
	{
	}

	// Convenince initializer
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name)
		: LabjackLogicalInputChannel(port_names, port_purpose, name, InputFinalDesiredValueType::Discrete, FinalDesiredValueLoggingMode::LoggedToAll, 1)
	{
	}
	

	bool getReturnsContinuousValue() override;
	std::string getCSVHeaderRepresentation() override;

	std::vector<std::string> getExpandedFinalValuePortNames();
	size_t getNumberOfDoubleInputs() const { return this->numDoubleInputValues; }
	void setNumberOfDoubleInputs(size_t numDoubles) { this->numDoubleInputValues = numDoubles; }


	std::function<unsigned(double, double)> fn_stream_timer = [](double upper_bits, double lower_bits)
	{
		// Combine SYSTEM_TIMER_20HZ's lower 16 bits and STREAM_DATA_CAPTURE_16, which
		// contains SYSTEM_TIMER_20HZ's upper 16 bits
		unsigned int timerValue = ((unsigned short)upper_bits << 16) +
			(unsigned short)lower_bits;
		return timerValue;
	};


	static std::bitset<8> convertValue_DigitalStateAsDigitalValues(double doubleRepresentation);
	static bool convertValue_AnalogAsDigitalInput(double analogValue);
	static unsigned int convertValue_StreamTimer(double upper_bits, double lower_bits);

protected:
	//std::function<bool> _fnDigitizeValue;
	size_t numDoubleInputValues; // The number of doubles this channel takes to produce its final value

	
};