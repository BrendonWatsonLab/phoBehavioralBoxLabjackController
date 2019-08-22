#include "StateMonitor.h"
#include <string>


StateMonitor::StateMonitor()
{
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		this->inputs[i] = new InputState("");
	}
}

StateMonitor::~StateMonitor()
{
	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		delete this->inputs[i];
	}
}

bool StateMonitor::refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS])
{
	bool hasAnyStateChanged = false;
	for (int i = 0; i < NUM_CHANNELS; i++)
	{
		bool hasThisStateChanged = this->inputs[i]->refresh(readTime, readValues[i]);
		hasAnyStateChanged = hasAnyStateChanged || hasThisStateChanged;
	}
	return hasAnyStateChanged; // returns true if any of the states has changed
}
