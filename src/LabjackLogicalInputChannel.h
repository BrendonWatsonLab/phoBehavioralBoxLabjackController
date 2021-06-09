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
	
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name, LabjackLogicalInputChannel::InputFinalDesiredValueType value_type, FinalDesiredValueLoggingMode logging_mode)
		: LabjackLogicalChannelBase(port_names, port_purpose, name, LabjackLogicalChannelBase::Direction::Input),
		  finalValueType(value_type), loggingMode(logging_mode)
	{
	}

	// Convenince initializer
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name)
		: LabjackLogicalInputChannel(port_names, port_purpose, name, InputFinalDesiredValueType::Discrete, FinalDesiredValueLoggingMode::LoggedToAll)
	{
	}
	

	bool getReturnsContinuousValue() override;
	std::string getCSVHeaderRepresentation() override;

	std::vector<std::string> getExpandedFinalValuePortNames();

protected:
	//std::function<bool> _fnDigitizeValue;

	
};