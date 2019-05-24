/**
 * Name: BehavioralBoxLabjackController.cpp
 * Desc: Main file
**/

//#ifdef _WIN32
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#else
//#include <arpa/inet.h>  // For inet_ntoa()
//#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <LabJackM.h>
#include <time.h>

//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"
#include "BehavioralBoxLabjack.h"


//// Scheduler
#include "External/Scheduler/Scheduler.h"
// number of tasks that can run simultaneously
  // Note: not the number of tasks that can be added,
  //       but number of tasks that can be run in parallel
unsigned int max_n_threads = 12;
// Make a new scheduling object.
  // Note: s cannot be moved or copied
Bosma::Scheduler s(max_n_threads);

bool isArtificialDaylightHours();
double SyncDeviceTimes(BehavioralBoxLabjack* labjack);
void updateVisibleLightRelayIfNeeded(BehavioralBoxLabjack* labjack);


int main()
{
	// Open first found LabJack
	//GetDeviceInfo("LJM_dtANY", "LJM_ctANY", "LJM_idANY");
	BehavioralBoxLabjack firstLabjack = BehavioralBoxLabjack(0, "LJM_dtANY", "LJM_ctANY", "LJM_idANY");

	SyncDeviceTimes(&firstLabjack);

	updateVisibleLightRelayIfNeeded(&firstLabjack);

	// Call the light relay updating function every hour
	//s.every(std::chrono::hours(1), updateVisibleLightRelayIfNeeded, &firstLabjack);



	// https://en.wikipedia.org/wiki/Cron
	s.cron("* * * * *", [&firstLabjack](BehavioralBoxLabjack* labjack) { updateVisibleLightRelayIfNeeded(labjack); }); //every minute

	// Every hour
	s.cron("0 * * * *", [&firstLabjack](BehavioralBoxLabjack* labjack) { updateVisibleLightRelayIfNeeded(labjack); }); //every hour


	//WaitForUserIfWindows();

	return LJME_NOERROR;
}


//// Syncs the Labjack's internal RTC time with the computer's. Returns the number of seconds that were adjusted to set the Labjack's clock.
double SyncDeviceTimes(BehavioralBoxLabjack* labjack) {
	int LJMError;
	time_t originalLabjackTime = labjack->getTime();
	LJMError = labjack->getError();
	printf("LABJACK TIME: %s\n", ctime(&originalLabjackTime));
	
	// Get the computer time:
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("COMPUTER TIME: %s\n", ctime(&computerTime));

	double updateChangeSeconds = difftime(computerTime, originalLabjackTime);

	if (updateChangeSeconds == 0) {
		printf("Computer time is already synced with Labjack time!\n");
	}
	else {
		printf("Computer time is %.f seconds from Labjack time...\n", updateChangeSeconds);
		// Write the computer time to the Labjack
		labjack->setTime(computerTime);
		LJMError = labjack->getError();

		// Re-read the time to confirm the update
		time_t updatedLabjackTime = labjack->getTime();
		LJMError = labjack->getError();
		printf("Updated Labjack TIME: %s\n", ctime(&updatedLabjackTime));
	}
	return updateChangeSeconds;
}

bool isArtificialDaylightHours() {
	time_t currTime = time(NULL);
	struct tm *currLocalTime = localtime(&currTime);

	int hour = currLocalTime->tm_hour;
	if ((hour < 6) || (hour > 18)) {
		// It's night-time
		return false;
	}
	else {
		// It's day-time
		return true;
	}	
}


void updateVisibleLightRelayIfNeeded(BehavioralBoxLabjack* labjack) {
	bool isDay = isArtificialDaylightHours();
	labjack->setVisibleLightRelayState(isDay);
}