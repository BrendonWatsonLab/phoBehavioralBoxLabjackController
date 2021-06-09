#pragma once
#include "LabjackLogicalChannelBase.h"

class LabjackLogicalInputChannel : public LabjackLogicalChannelBase
{
public:

	enum class InputFinalDesiredValueType { Discrete, Continuous };

	InputFinalDesiredValueType finalValueType;
	
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name, LabjackLogicalInputChannel::InputFinalDesiredValueType value_type)
		: LabjackLogicalChannelBase(port_names, port_purpose, name, LabjackLogicalChannelBase::Direction::Input),
		  finalValueType(value_type)
	{
	}

	// Convenince initializer
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose, const std::string& name)
		: LabjackLogicalInputChannel(port_names, port_purpose, name, InputFinalDesiredValueType::Discrete)
	{
	}
	

	bool getReturnsContinuousValue() override;
	std::string getCSVHeaderRepresentation() override;
protected:
	//std::function<bool> _fnDigitizeValue;

	
};