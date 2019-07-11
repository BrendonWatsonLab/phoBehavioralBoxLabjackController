#pragma once
//config.h:
/*
Specifies what ports to open
*/

// number of tasks that can run simultaneously
#define max_n_threads 3

// Number of Labjack Channels
//enum { NUM_CHANNELS = 11 };
//#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }

#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }
//#define globalLabjackInputPortNames { "EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7","MIO0"}
//#define globalLabjackInputPortNames {  }
enum { NUM_CHANNELS = 9 };
//enum { NUM_CHANNELS = 0 };

//#define globalLabjackLightRelayPortName "MIO2" // Defines the output port that is used to control the relay that switches the lighting on/off.
#define globalLabjackLightRelayPortName "FIO2" // Defines the output port that is used to control the relay that switches the lighting on/off. 



//#define globalLabjackInputPortNames { "AIN0" }
//enum { NUM_CHANNELS = 1 };
