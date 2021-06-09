#include "InputState.h"

#include <utility>

#include "LabjackLogicalInputChannel.h"
#include "LabjackPortType.h"


InputState::InputState(const LabjackLogicalInputChannel& channelRef)
{
}


InputState::~InputState()
{
}

bool InputState::refresh(std::chrono::time_point<Clock> refreshTime, std::vector<double> expandedFinalValues)
{
	bool hasChangeOccured = false;
	if (this->_isUninitialized)
	{
		// First initialization
		this->_previousExpandedFinalValues = std::move(expandedFinalValues);
		this->_isUninitialized = false;
		hasChangeOccured = true; // if we're initializing for the first time, the value has changed
	}

	//TODO: Need to use the proper comparison method depending on the type of logical channel we're looking at.
	// e.g. A AnalogAsDigital only needs to see if both are equal element-wise (are both 0.0 at each element, 1.0 at each element)
	// e.g. An Analog continuous pin needs to see if each element falls within a tolerance of the same element in the previous vector

	
	expandedFinalValues.
	
	

	// Update the refresh time
	this->last_refresh_time = refreshTime;
	
	return false;
}

//bool InputState::refresh(std::chrono::time_point<Clock> refreshTime, double readValue)
//{
//	this->last_refresh_time = refreshTime;
//	if (this->presumedPinState != (readValue >= 0.5)) {
//		// A change occured
//		// Deal with the old state:
//
//
//		// Update the state:
//		this->presumedPinState = (readValue >= 0.5);
//		this->last_change_time = refreshTime;
//
//		// Return true to indicate that the input state has changed.
//		return true;
//	}
//	else {
//		// input state hasn't changed
//		return false;
//	}
//}
