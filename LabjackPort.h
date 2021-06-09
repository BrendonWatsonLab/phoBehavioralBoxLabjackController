#pragma once
#include <iostream>
#include <cstdint>
#include <string>

struct LabjackPort
{
	enum class Direction { Input, Output };
	//enum direction { left = 'l', right = 'r' };

	//enum class PortType { Analog, Digital, DigitalState };


	int address;
	std::string name;
	
	Direction direction;

	// Optional Properties:
	std::string purpose = "";



	// Functions

	// Gets the representation of this item to render in the CSV header
	std::string getCSVHeaderRepresentation();

	
	
	
	
};
