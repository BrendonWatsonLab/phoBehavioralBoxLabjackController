#pragma once
//config.h:
/*
Specifies what ports to open
*/

// number of tasks that can run simultaneously
#define max_n_threads 3

// The maximum number of simultaneous labjacks that can be connected to this computer
#define max_number_labjacks 8

////// Daylight Cycle Settings:
// globalDaylightStartHour: defines the hour of the day (in a 24-hour format) at which the Visible LEDS are turned on (illuminated) (simulating daylight for the mouse).
#define globalDaylightStartHour 6
////Ex: 6: The lights turn on at 6am and remain on until the subsequent globalDaylightOffHour
// globalDaylightOffHour: defines the hour of the day (in a 24-hour format) at which the Visible LEDS are turned off (simulating nighttime for the mouse).
#define globalDaylightOffHour 18
////Ex: 18: The lights turn off at 6pm and remain off until the subsequent globalDaylightStartHour


////// GUI Config:
#define PRINT_OUTPUT_VALUES_TO_CONSOLE false

#define LAUNCH_WEB_SERVER true
#define ENABLE_WEB_SERVER_LIVE_WIDGET false

#define LOAD_HISTORICAL_DATA true



////// DEBUGGING ONLY:
//CONTINUE_WITHOUT_LABJACKS: if false, it waits on at least one labjack hardware to be detected before moving forward.
#define CONTINUE_WITHOUT_LABJACKS false

////// Labjack Port Configurations:
// Only one of these should be un-commented at a time.
//#include "../ConfigFiles/config-DownstairsBox.h"
//// -- OR --
#include "../ConfigFiles/config-UpstairsBox-8-20-2019.h"