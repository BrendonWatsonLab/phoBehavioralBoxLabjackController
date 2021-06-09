#include "InputState.h"

#include <utility>
#include <algorithm> // Used for std::find

#include "LabjackLogicalInputChannel.h"
#include "LabjackPortType.h"


InputState::InputState(const LabjackLogicalInputChannel& channelRef) : fn_get_didInputValueChange(channelRef.fn_generic_get_didValueChange)
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

	if (!hasChangeOccured) {
		// If we haven't already detected a change, try calling the function
		if (this->fn_get_didInputValueChange)
		{
			auto didChangesOccur = this->fn_get_didInputValueChange(expandedFinalValues.size(), this->_previousExpandedFinalValues.data(), expandedFinalValues.data());
			//didChangesOccur
			if (std::find(didChangesOccur.begin(), didChangesOccur.end(), true) != didChangesOccur.end()) {
				hasChangeOccured = hasChangeOccured || true;
			}			
		} // Otherwise the function isn't set!
	}

	// Update the refresh time
	this->last_refresh_time = refreshTime;

	if (hasChangeOccured)
	{
		this->last_change_time = refreshTime;
		this->_previousExpandedFinalValues = expandedFinalValues; // update the previous values to be the current values before returning true
	}
	// Return true to indicate that the input state has changed, or false to indicate that it hasn't
	return hasChangeOccured;
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
