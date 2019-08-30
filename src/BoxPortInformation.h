#pragma once
#include <type_traits>
#include <vector>
#include <string>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

/*! \brief Contains the information associated with a variable parsed fromthe Labjack CSV
 */
struct BoxPortInformation {

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


	std::string portName;
	std::string portPurpose;

	BehavioralPortDispenseTypes dispenseType;
	BehavioralEventKind eventKind;
	PortLocation portLocation;

	Wt::WColor color = Wt::WColor(0, 255, 255, 255);

	BoxPortInformation(std::string portName, std::string portPurpose, BehavioralPortDispenseTypes dispenseType, BehavioralEventKind eventKind, PortLocation portLocation, Wt::WColor color) : portName(portName), portPurpose(portPurpose), dispenseType(dispenseType), eventKind(eventKind), portLocation(portLocation), color(color)
	{

	}

	// Get the y-axis height of the impulse bar on the graph
	double getDataYPosition() {
		switch (this->eventKind)
		{
		case BehavioralEventKind::BeamBreak:
			return 1.5;
			break;
		case BehavioralEventKind::Dispense:
			return 5.5;
			break;
		default:
			return 7.5;
			break;
		}
	}

};
