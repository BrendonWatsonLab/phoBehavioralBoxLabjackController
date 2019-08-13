#pragma once
//config.h:
/*
Specifies what ports to open
*/

// number of tasks that can run simultaneously
#define max_n_threads 3

// The maximum number of simultaneous labjacks that can be connected to this computer
#define max_number_labjacks 8

// Number of Labjack Channels
//enum { NUM_CHANNELS = 11 };
//#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }

//#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }
#define globalLabjackInputPortNames { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7","MIO0"}

//#define globalLabjackInputPortNames { "FIO0","FIO1","FIO2","FIO3","AIN0","AIN1","AIN2","AIN3","MIO0" }

enum { NUM_CHANNELS = 9 };
#define globalLabjackInputPortPurpose { "Water1_BeamBreak","Water2_BeamBreak","Food1_BeamBreak","Food2_BeamBreak","Water1_Dispense","Water2_Dispense","Food1_Dispense","Food2_Dispense","MIO0"}

#define globalLabjackOutputPortNames { "MIO2","FIO0","FIO1" }
//#define globalLabjackOutputPortNames { "FIO2" }
enum { NUM_OUTPUT_CHANNELS = 3 };

//#define globalLabjackLightRelayPortName "MIO2" // Defines the output port that is used to control the relay that switches the lighting on/off. 
//#define globalLabjackLightRelayPortName "CIO0" // Defines the output port that is used to control the relay that switches the lighting on/off. 


//#define globalLabjackInputPortNames { "AIN0" }
//enum { NUM_CHANNELS = 1 };


# define waterPort1_Index 