#include "OutputState.h"

OutputState::OutputState()
{
}

OutputState::OutputState(std::string pinName) : pinName{ pinName }
{
}


OutputState::~OutputState()
{
}

bool OutputState::set(std::chrono::time_point<Clock> refreshTime, double readValue)
{
	this->last_set_time = refreshTime;
	if (this->presumedPinState != (readValue >= 0.5)) {
		// A change occured
		// Deal with the old state:


		// Update the state:
		this->presumedPinState = (readValue >= 0.5);
		this->last_change_time = refreshTime;

		// Return true to indicate that the input state has changed.
		return true;
	}
	else {
		// input state hasn't changed
		return false;
	}
}
