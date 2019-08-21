#pragma once
//config-DownstairsBox.h:
/*
Specifies what ports to open. Imported by config.h.
*/


//// Inputs:
// Analog Breakout Board:
enum { NUM_CHANNELS = 9 };
#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }
#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense","RunningWheel_Beambreak"}

//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 3 };
#define globalLabjackOutputPortNames { "MIO2","MIO1","MIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay","WaterPortAttractModeLED","WaterPortAttractModeLED"}
