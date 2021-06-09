#pragma once
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

protected:
	//std::function<bool> _fnDigitizeValue;
	size_t numDoubleInputValues; // The number of doubles this channel takes to produce its final value

	
};