#pragma once
//config-PhoHomeTesting-06-02-2021.h:
/*
Specifies what ports to open. Imported by config.h.
*/

// All:
enum { NUM_CHANNELS = 5 };
#define globalLabjackInputPortNames { "AIN0","AIN1","AIN2","AIN3","FIO_STATE"} // FIO_STATE: Read the state of the 8 bits of FIO in a single binary-encoded value.
#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","SIGNALS_Dispense"}
#define globalLabjackInputPortIsAnalog { true,true,true,true,false}
// Digital Only:
enum { NUM_CHANNELS_DIGITAL = 1 };
#define globalLabjackDigitalInputPortNames { "FIO_STATE"}
#define globalLabjackDigitalInputPortPurpose { "SIGNALS_Dispense"}
// Analog Only:
enum { NUM_CHANNELS_ANALOG = 4 };
#define globalLabjackAnalogInputPortNames {"AIN0","AIN1","AIN2","AIN3"}
#define globalLabjackAnalogInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak"}


//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 1 };
#define globalLabjackOutputPortNames { "CIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay" }

// File Locations:
#define globalOutputFileDirectory "C:/Common/data/"
#define globalHistoricalFileSearchDirectory globalOutputFileDirectory
