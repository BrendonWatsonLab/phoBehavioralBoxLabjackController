#pragma once
#include <time.h>
#include <chrono>
#include <vector>

#include "config.h"
#include "InputState.h"

// Simple forward declaration
//struct LabjackStreamInfo;

// Keep track of the presumed current state of each port. 
// Upon each read, check if the read values differ from the previous values. When a change occurs:
	// Capture the current timestamp and the new value
	// return true to indicate that the values have changed.
typedef std::chrono::system_clock Clock;

class StateMonitor
{
public:
	StateMonitor(std::vector<LabjackLogicalInputChannel*> input_channels);
	~StateMonitor();

	// returns true if any state has been changed
	//bool refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS]);

	//bool refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS], LabjackPortType readPortType[NUM_CHANNELS]);

	//bool refreshState(std::chrono::time_point<Clock> readTime, LabjackStreamInfo* readStreamState);

	bool refreshState(std::chrono::time_point<Clock> readTime, std::vector<std::vector<double>> readInputExpandedValues);

	size_t getNumberOfInputs() const { return this->inputs.size(); }

private:
	std::vector<InputState*> inputs;
	//InputState* inputs[NUM_CHANNELS];

};

