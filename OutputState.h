#pragma once
#include <time.h>
#include <chrono>
#include <string>
typedef std::chrono::system_clock Clock;

// The hardware output state of a pin
class OutputState
{
public:
	//OutputState();
	OutputState(std::string pinName);
	~OutputState();

	// returns true if the output state has been changed
	bool set(std::chrono::time_point<Clock> refreshTime, double readValue);

	std::string getPinName() { return this->pinName;  }

private:
	std::string pinName = "ERR";
	bool presumedPinState = false; //Either high (true) or false (low)

	std::chrono::time_point<Clock> last_change_time = Clock::now(); // The most recent time that the pin's value was changed.
	std::chrono::time_point<Clock> last_set_time = Clock::now(); // The most recent time the pin's value was refreshed (whether it changed or not)
};

