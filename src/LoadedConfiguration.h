#pragma once

struct LoadedConfiguration
{

	int daylightStartHour = 6;
	int daylightOffHour = 18;
	bool launch_web_server = true;

	bool shouldEnableSynchronize_Y_Axis = false;
	int numDaysToDisplay = 60;
};