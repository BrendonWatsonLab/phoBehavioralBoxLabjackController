#pragma once
//config-AnalogRunningWheel-12-17-2019.h:
/*
Specifies what ports to open. Imported by config.h.
*/


//// Inputs:
// Digital Breakout Board:
//enum { NUM_CHANNELS = 9 };
//#define globalLabjackInputPortNames { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7","AIN0"}
//#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense","RunningWheel"}

// All:
enum { NUM_CHANNELS = 9 };
#define globalLabjackInputPortNames { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7","AIN0"}
#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense","RunningWheel"}
#define globalLabjackInputPortIsAnalog { false,false,false,false,false,false,false,false,true}
// Digital Only:
enum { NUM_CHANNELS_DIGITAL = 8 };
#define globalLabjackDigitalInputPortNames { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7"}
#define globalLabjackDigitalInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense"}
// Analog Only:
enum { NUM_CHANNELS_ANALOG = 1 };
#define globalLabjackAnalogInputPortNames {"AIN0"}
#define globalLabjackAnalogInputPortPurpose { "RunningWheel"}



//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 1 };
#define globalLabjackOutputPortNames { "CIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay" }

// File Locations:
#define globalOutputFileDirectory "C:/Common/data/"
#define globalHistoricalFileSearchDirectory globalOutputFileDirectory
