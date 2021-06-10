#pragma once
//config-PhoHomeTesting-06-02-2021.h:
/*
Specifies what ports to open. Imported by config.h.
UPDATE: On 06-10-2021 this was changed to be instead read in dynamically from the config .json file, by default in C:\Common\Config\*.json
*/

//// Outputs:
enum { NUM_OUTPUT_CHANNELS = 1 };
#define globalLabjackOutputPortNames { "CIO1" }
#define globalLabjackOutputPortPurpose { "VisibleLedRelay" }

// File Locations:
#define globalOutputFileDirectory "C:/Common/data/"
#define globalHistoricalFileSearchDirectory globalOutputFileDirectory
