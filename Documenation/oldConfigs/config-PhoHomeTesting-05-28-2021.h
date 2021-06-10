#pragma once
//config-PhoHomeTesting-05-28-2021.h:
/*
Specifies what ports to open. Imported by config.h.
*/

// All:
enum { NUM_CHANNELS = 8 };
#define globalLabjackInputPortNames { "AIN0","AIN1","AIN2","AIN3","FIO0","FIO1","FIO2","FIO3"}
#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense"}
#define globalLabjackInputPortIsAnalog { true,true,true,true,false,false,false,false}
// Digital Only:
enum { NUM_CHANNELS_DIGITAL = 4 };
#define globalLabjackDigitalInputPortNames { "FIO0","FIO1","FIO2","FIO3"}
#define globalLabjackDigitalInputPortPurpose { "Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense"}
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
