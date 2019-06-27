#pragma once
#include <time.h>
#include <chrono>

#include "src/config.h"
#include "InputState.h"

// Keep track of the presumed current state of each port. 
// Upon each read, check if the read values differ from the previous values. When a change occurs:
	// Capture the current timestamp

	//

	// Write the previous values out to the file
typedef std::chrono::system_clock Clock;

class StateMonitor
{
public:
	StateMonitor();
	~StateMonitor();

	// returns true if any state has been changed
	bool refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS]);

private:
	InputState* inputs[NUM_CHANNELS];

};

