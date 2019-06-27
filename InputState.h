#pragma once
#include <time.h>
#include <chrono>
#include <string>

// Manages the input state for a single pin. Responsible for detecting changes.
typedef std::chrono::system_clock Clock;

class InputState
{
public:
	InputState();
	InputState(std::string pinName);
	~InputState();

	// returns true if the input state has been changed
	bool refresh(std::chrono::time_point<Clock> refreshTime, double readValue);

	std::string pinName = "";
	bool presumedPinState = false; //Either high (true) or false (low)
	std::chrono::time_point<Clock> last_change_time = Clock::now(); // The most recent time that the pin's value was changed.
	std::chrono::time_point<Clock> last_refresh_time = Clock::now(); // The most recent time the pin's value was refreshed (whether it changed or not)
};

