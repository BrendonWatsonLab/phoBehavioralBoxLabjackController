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

//#define globalLabjackInputPortNames { "DIO0","DIO1","DIO2","DIO3","DIO4","DIO5","DIO6","DIO7","MIO0" }
//enum { NUM_CHANNELS = 11 };



#define globalLabjackInputPortNames { "AIN0" }
enum { NUM_CHANNELS = 1 };
