#pragma once
#include <vector>
#include <string>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

/*! \brief Enumeration of the substance being dispensed
 */
enum class BehavioralPortDispenseTypes {
	Food,
	Water,
	Light,    
	None
};

/*! \brief Enumeration of the kind of event thats occuring
 */
enum class BehavioralEventKind {
	BeamBreak,  
	Dispense,
	Other
};

/*! \brief Enumeration of the four ports that poke into the mouse's cage.
 */
enum class PortLocation {
	Water1,
	Water2,
	Food1,
	Food2,
	None
};

/*! \brief Contains the information associated with a variable parsed fromthe Labjack CSV
 */
struct BoxPortInformation {
	std::string portName;
	std::string portPurpose;

	BehavioralPortDispenseTypes dispenseType;
	BehavioralEventKind eventKind;
	PortLocation portLocation;

	Wt::WColor color = Wt::WColor(0, 255, 255, 255);

	BoxPortInformation(std::string portName, std::string portPurpose, BehavioralPortDispenseTypes dispenseType, BehavioralEventKind eventKind, PortLocation portLocation, Wt::WColor color) : portName(portName), portPurpose(portPurpose), dispenseType(dispenseType), eventKind(eventKind), portLocation(portLocation), color(color)
	{

	}

};
