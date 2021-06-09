﻿#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <functional>
#include <utility>

class LabjackLogicalChannelBase
{
public:
	enum class Direction { Input, Output };

	//enum direction { left = 'l', right = 'r' };

	//enum class PortType { Analog, Digital, DigitalState };



	
	std::vector<std::string> portNames;
	std::vector<int> portAddresses;
	
	// Optional Properties:
	std::vector<std::string> portPurpose;

	LabjackLogicalChannelBase(std::vector<std::string> port_names, const std::vector<std::string>& port_purpose,
	                      const std::string& name, Direction direction);

	// simple getters:
	std::string getName() const
	{
		return this->name;
	}

	LabjackLogicalChannelBase::Direction getDirection() const
	{
		return this->direction;
	}

	
	bool getReturnsContinuousValue();
	std::vector<std::string> getPortNames() { return this->portNames; };
	//std::vector<std::string> getPortPurposes();


	// Functions

	// Gets the representation of this item to render in the CSV header
	std::string getCSVHeaderRepresentation();


	


protected:
	std::string name;
	Direction direction;
	
	std::function<std::vector<std::string>()> _fnLabjackPortNames;
	//std::function<double()> visibleLEDRelayFunction = [=]() -> double {
	//	if (this->isVisibleLEDLit()) { return 0.0; }
	//	else { return 1.0; }
	//};
};





