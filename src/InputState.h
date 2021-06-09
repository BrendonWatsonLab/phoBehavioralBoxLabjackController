#pragma once
#include <time.h>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

//TODO: Enable analog mode "tolerances" for determining if value changed.
//TODO: Enable analog mode "thresholds/ranges" for determining if value changed.
//TODO: Enable custom port names

class LabjackLogicalInputChannel;

// Manages the input state for a single pin. Responsible for detecting changes.
typedef std::chrono::system_clock Clock;

class InputState
{
public:
	InputState(const LabjackLogicalInputChannel& channelRef);
	~InputState();

	// Primary refresh function: returns true if the state has changed since the last time it was refreshed
	// returns true if the input state has been changed
	bool refresh(std::chrono::time_point<Clock> refreshTime, std::vector<double> expandedFinalValues);

	

	// Timekeeping variables:
	std::chrono::time_point<Clock> last_change_time = Clock::now(); // The most recent time that the pin's value was changed.
	std::chrono::time_point<Clock> last_refresh_time = Clock::now(); // The most recent time the pin's value was refreshed (whether it changed or not)

private:
	bool _isUninitialized = true; // true until the first refresh(...) operation completes
	std::vector<double> _previousExpandedFinalValues;

	std::function<std::vector<bool>(int, double*, double*)> fn_get_didInputValueChange;
	
};

