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
#include <iostream>
#include <fstream>
#include <vector>

//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"
#include "BehavioralBoxLabjack.h"

#include "LabjackHelpers.h"

//#include "../../C_C++_LJM_2019-05-20/LJM_Utilities.h"

// File Output:
std::ofstream out_stream("out_fle.csv");
//out_stream.open("out_file.csv");

//BehavioralBoxLabjack firstLabjack = BehavioralBoxLabjack(0, "LJM_dtANY", "LJM_ctANY", "LJM_idANY", out_stream);
std::vector<BehavioralBoxLabjack*> foundLabjacks;


//// Scheduler
#include "External/Scheduler/Scheduler.h"
// number of tasks that can run simultaneously
  // Note: not the number of tasks that can be added,
  //       but number of tasks that can be run in parallel
unsigned int max_n_threads = 12;
// Make a new scheduling object.
  // Note: s cannot be moved or copied
Bosma::Scheduler s(max_n_threads);

// FUNCTION PROTOTYPES:

void runTopOfHourUpdate();
void runTopOfMinuteUpdate();
void runTopOfSecondUpdate();

bool isArtificialDaylightHours();
double SyncDeviceTimes(BehavioralBoxLabjack* labjack);
void updateVisibleLightRelayIfNeeded(BehavioralBoxLabjack* labjack);


int main()
{
	// Find the labjacks
	foundLabjacks = LabjackHelpers::findAllLabjacks();

	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		//SyncDeviceTimes(&foundLabjacks[i]);
		//updateVisibleLightRelayIfNeeded(&foundLabjacks[i]);
	}

	// Open first found LabJack
	//GetDeviceInfo("LJM_dtANY", "LJM_ctANY", "LJM_idANY");

	/*SyncDeviceTimes(&firstLabjack);

	updateVisibleLightRelayIfNeeded(&firstLabjack);*/

	// Call the light relay updating function every hour
	//s.every(std::chrono::seconds(1), runTopOfSecondUpdate);

	// https://en.wikipedia.org/wiki/Cron
	//s.cron("* * * * *", [&firstLabjack](BehavioralBoxLabjack* labjack) { updateVisibleLightRelayIfNeeded(labjack); }); //every minute

	// Every hour
	//s.cron("0 * * * *", [&firstLabjack](BehavioralBoxLabjack* labjack) { updateVisibleLightRelayIfNeeded(labjack); }); //every hour

	// Ran at the top of every hour
	s.cron("0 * * * *", []() { runTopOfHourUpdate(); });
	// Ran at the top of every minute
	s.cron("* * * * *", []() { runTopOfMinuteUpdate(); });



	// TODO - READ ME: main run loop
		// The LJM_StartInterval, LJM_WaitForNextInterval, and LJM_CleanInterval functions are used to efficiently execute the loop every so many milliseconds
		// To permit multiple labjacks operating in a general way, we probably want this loop to be contained within a thread that is owned by the BehavioralBoxLabjack object.
		// This main loop will simplely 
		// Each BehavioralBoxLabjack also needs to be responsible for writing out its own file.
		// There are some archetecture decisions to be made.
		// Perhaps turning the lights on and off should belong to the individual boxes as well.
		// Main should have perhaps an array of things?

	//WaitForUserIfWindows();
	// Main run loop:
	int terminateExecution = 0;
	
	while (terminateExecution != 1) {
		/*
			Here we will perform the reading of the LabJack inputs (sensor values, etc).
		*/


	}

	out_stream.close();

	return LJME_NOERROR;
}

// Ran at the top of every hour
void runTopOfHourUpdate() {
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("runHourlyLightsUpdate: running at %s\n", ctime(&computerTime));
	//updateVisibleLightRelayIfNeeded(&firstLabjack);
		// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfMinuteUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		updateVisibleLightRelayIfNeeded(foundLabjacks[i]);
	}
}

// Ran at the top of every minute
void runTopOfMinuteUpdate() {
	time_t computerTime;
	time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
	printf("runTopOfMinuteUpdate: running at %s\n", ctime(&computerTime));
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfMinuteUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		foundLabjacks[i]->readSensorValues();
	}
	
}

// Ran at the top of every second
void runTopOfSecondUpdate() {
	time_t computerTime;
	// Iterate through all found Labjacks
	for (int i = 0; i < foundLabjacks.size(); i++) {
		time(&computerTime);  /* get current time; same as: timer = time(NULL)  */
		printf("runTopOfSecondUpdate: running at %s for labjack %i\n", ctime(&computerTime), i);
		foundLabjacks[i]->readSensorValues();
	}

	
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


