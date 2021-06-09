#pragma once
#include "LabjackLogicalChannelBase.h"

class LabjackLogicalInputChannel : public LabjackLogicalChannelBase
{
public:

	enum class InputFinalDesiredValueType { Discrete, Continuous };

	
	LabjackLogicalInputChannel(const std::vector<std::string>& port_names, const std::vector<std::string>& port_purpose,
		const std::string& name)
		: LabjackLogicalChannelBase(port_names, port_purpose, name, LabjackLogicalChannelBase::Direction::Input)
	{
	}

	bool getReturnsContinuousValue() override;
	std::string getCSVHeaderRepresentation() override;
protected:
	//std::function<bool> _fnDigitizeValue;

	
};