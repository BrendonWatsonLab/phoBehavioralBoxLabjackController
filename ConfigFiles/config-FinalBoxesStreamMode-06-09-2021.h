#pragma once
//config-PhoHomeTesting-06-02-2021.h:
/*
Specifies what ports to open. Imported by config.h.
*/

// All:
enum { NUM_CHANNELS = 4 };
#define globalLabjackInputPortNames { "EIO_STATE","AIN0","SYSTEM_TIMER_20HZ","STREAM_DATA_CAPTURE_16"} // FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
#define globalLabjackInputPortPurpose {"SIGNALS_All","RunningWheel","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
#define globalLabjackInputPortIsAnalog { false,true,false,false}
#define globalLabjackInputPortType {LabjackPortType::DigitalState, LabjackPortType::Analog, LabjackPortType::DigitalState, LabjackPortType::DigitalState}
#define globalLabjackInputPortIsLogged { true,true,false,false}

// Digital Only:
enum { NUM_CHANNELS_DIGITAL = 3 };
#define globalLabjackDigitalInputPortNames { "EIO_STATE","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
#define globalLabjackDigitalInputPortPurpose { "SIGNALS_All","SYSTEM_TIMER_20HZ", "STREAM_DATA_CAPTURE_16"}
#define globalLabjackDigitalInputPortType {LabjackPortType::DigitalState, LabjackPortType::DigitalState, LabjackPortType::DigitalState}
#define globalLabjackDigitalInputPortIsLogged { true,false,false}

// Analog Only:
enum { NUM_CHANNELS_ANALOG = 1 };
#define globalLabjackAnalogInputPortNames {"AIN0"}
#define globalLabjackAnalogInputPortPurpose { "RunningWheel"}
#define globalLabjackAnalogInputPortType {LabjackPortType::Analog}

//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 1 };
#define globalLabjackOutputPortNames { "CIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay" }

// File Locations:
#define globalOutputFileDirectory "C:/Common/data/"
#define globalHistoricalFileSearchDirectory globalOutputFileDirectory
