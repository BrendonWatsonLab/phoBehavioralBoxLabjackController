#pragma once
//config-PhoHomeTesting-06-02-2021.h:
/*
Specifies what ports to open. Imported by config.h.
*/

// All:
//enum { NUM_CHANNELS = 7 };
//#define globalLabjackInputPortNames { "AIN0","AIN1","AIN2","AIN3","FIO_STATE","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"} // FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
//#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","SIGNALS_Dispense","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
//#define globalLabjackInputPortIsAnalog { true,true,true,true,false,false,false}
//#define globalLabjackInputPortType {LabjackPortType::Analog, LabjackPortType::Analog, LabjackPortType::Analog, LabjackPortType::Analog, LabjackPortType::DigitalState, LabjackPortType::DigitalState, LabjackPortType::DigitalState}
//#define globalLabjackInputPortIsLogged { true,true,true,true,true,false,false}

//// Digital Only:
//enum { NUM_CHANNELS_DIGITAL = 3 };
//#define globalLabjackDigitalInputPortNames { "FIO_STATE","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
//#define globalLabjackDigitalInputPortPurpose { "SIGNALS_Dispense","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
//#define globalLabjackDigitalInputPortType {LabjackPortType::DigitalState, LabjackPortType::DigitalState, LabjackPortType::DigitalState}
//#define globalLabjackDigitalInputPortIsLogged { true,false,false}
//
//// Analog Only:
//enum { NUM_CHANNELS_ANALOG = 4 };
//#define globalLabjackAnalogInputPortNames {"AIN0","AIN1","AIN2","AIN3"}
//#define globalLabjackAnalogInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak"}
//#define globalLabjackAnalogInputPortType {LabjackPortType::Analog, LabjackPortType::Analog, LabjackPortType::Analog, LabjackPortType::Analog}

//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 1 };
#define globalLabjackOutputPortNames { "CIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay" }

// File Locations:
#define globalOutputFileDirectory "C:/Common/data/"
#define globalHistoricalFileSearchDirectory globalOutputFileDirectory
