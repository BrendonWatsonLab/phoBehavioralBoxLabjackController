#include "StateMonitor.h"
#include <string>


//StateMonitor::StateMonitor()
//{
//	LabjackPortType curr_port_types[NUM_CHANNELS] = globalLabjackInputPortType;
//	int active_input_state_index = 0;
//	for(int i = 0; i < NUM_CHANNELS; i++)
//	{
//		LabjackPortType curr_port_type = curr_port_types[i];
//		switch (curr_port_type)
//		{
//		case LabjackPortType::Analog:
//			this->inputs[active_input_state_index] = new InputState("", curr_port_type);
//			break;
//		case LabjackPortType::Digital:
//			this->inputs[active_input_state_index] = new InputState("", curr_port_type);
//			break;
//		case LabjackPortType::DigitalState:
//			//for (int j = 0; j < 8; j++) {
//			//	active_input_state_index = active_input_state_index + j;
//				this->inputs[active_input_state_index] = new InputState("", curr_port_type);
//			//}
//			break;
//		default: 
//			break;
//		}
//		
//		active_input_state_index++; // Update to the next location
//		
//	}
//}

StateMonitor::StateMonitor(std::vector<LabjackLogicalInputChannel*> input_channels)
{
	size_t newNumberOfInputs = input_channels.size();
	
	this->inputs = std::vector<InputState*>(newNumberOfInputs); // pre-allocate the vector
	for (int i = 0; i < newNumberOfInputs; i++)
	{
		const auto currInputChannel = input_channels[i];
		//this->inputs[i] = new InputState(*currInputChannel);
		this->inputs[i] = new InputState(*currInputChannel);
	}
}



StateMonitor::~StateMonitor()
{
	for (int i = 0; i < this->getNumberOfInputs(); i++)
	{
		delete this->inputs[i];
		this->inputs[i] = nullptr;
	}
	this->inputs.clear();
}

bool StateMonitor::refreshState(std::chrono::time_point<Clock> readTime, std::vector<std::vector<double>> readInputExpandedValues)
{
	bool hasAnyStateChanged = false;
	for (int i = 0; i < this->getNumberOfInputs(); i++)
	{
		bool hasThisStateChanged = this->inputs[i]->refresh(readTime, readInputExpandedValues[i]);
		hasAnyStateChanged = hasAnyStateChanged || hasThisStateChanged;
	}
	return hasAnyStateChanged;
}

//bool StateMonitor::refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS])
//{
//	bool hasAnyStateChanged = false;
//	for (int i = 0; i < NUM_CHANNELS; i++)
//	{
//		bool hasThisStateChanged = this->inputs[i]->refresh(readTime, readValues[i]);
//		hasAnyStateChanged = hasAnyStateChanged || hasThisStateChanged;
//	}
//	return hasAnyStateChanged; // returns true if any of the states has changed
//}
//
//
//bool StateMonitor::refreshState(std::chrono::time_point<Clock> readTime, double readValues[NUM_CHANNELS], LabjackPortType readPortType[NUM_CHANNELS])
//{
//	return false;
//}
//
//bool StateMonitor::refreshState(std::chrono::time_point<Clock> readTime, LabjackStreamInfo* readStreamState)
//{
//	bool hasAnyStateChanged = false;
//	
//	return false;
//}
