#include "LoadedConfiguration.h"

#include "LabjackLogicalInputChannel.h"

//
// LoadedConfiguration
//
//bool LoadedConfiguration::persistToIniFile(std::string path)
//{
//	return false;
//}



//
// LoadedLogicalChannelsSetupConfiguration
//
std::vector<LabjackLogicalInputChannel*> LoadedLogicalChannelsSetupConfiguration::buildLogicalInputChannels()
{
	std::vector<LabjackLogicalInputChannel*> logicalInputChannels;

	for (int i = 0; i < this->logicalChannelConfigs.size(); ++i)
	{
		auto active_config = this->logicalChannelConfigs[i];
		LabjackLogicalInputChannel* aNewInputChannel = new LabjackLogicalInputChannel(active_config.portNames, active_config.portPurpose, active_config.name);

		switch (active_config.channelValuesMode)
		{
		case LoadedLogicalChannelConfiguration::CVM_AnalogAsDigitalInput: 
			aNewInputChannel->fn_generic_get_value = LabjackLogicalInputChannel::getDefault_genericGetValueFcn_AnalogAsDigitalInput();
			aNewInputChannel->fn_generic_get_didValueChange = LabjackLogicalInputChannel::getDefault_didChangeFcn_AnalogAsDigitalInput();
			break;
		case LoadedLogicalChannelConfiguration::CVM_DigitalStateAsDigitalValues: 
			aNewInputChannel->fn_generic_get_value = LabjackLogicalInputChannel::getDefault_genericGetValueFcn_DigitalStateAsDigitalValues();
			aNewInputChannel->fn_generic_get_didValueChange = LabjackLogicalInputChannel::getDefault_didChangeFcn_DigitalStateAsDigitalValues();
			break;
		case LoadedLogicalChannelConfiguration::CVM_AnalogAsContinuousInput:
			aNewInputChannel->finalValueType = LabjackLogicalInputChannel::InputFinalDesiredValueType::Continuous;

			aNewInputChannel->fn_generic_get_value = LabjackLogicalInputChannel::getDefault_genericGetValueFcn_AnalogAsContinuousInput();
			aNewInputChannel->fn_generic_get_didValueChange = LabjackLogicalInputChannel::getDefault_didChangeFcn_AnalogAsContinuousInput();
			break;
		case LoadedLogicalChannelConfiguration::CVM_TimerRegistersAsContinuousTimer:
			aNewInputChannel->loggingMode = LabjackLogicalInputChannel::FinalDesiredValueLoggingMode::NotLogged;
			aNewInputChannel->setNumberOfDoubleInputs(2); // Takes 2 double values to produce its output
			aNewInputChannel->fn_get_expanded_port_names = LabjackLogicalInputChannel::getDefault_getExpandedPortNamesFcn_TimerRegistersAsContinuousTimer();
			aNewInputChannel->fn_generic_get_value = LabjackLogicalInputChannel::getDefault_genericGetValueFcn_TimerRegistersAsContinuousTimer();
			aNewInputChannel->fn_generic_get_didValueChange = LabjackLogicalInputChannel::getDefault_didChangeFcn_TimerRegistersAsContinuousTimer();
			break;
		case LoadedLogicalChannelConfiguration::CVM_Custom: 
			//TODO: This needs to be handled/implemented

			break;
		
		default: ;
		}
		
		logicalInputChannels.push_back(aNewInputChannel);
	}
	
	return logicalInputChannels;
}
