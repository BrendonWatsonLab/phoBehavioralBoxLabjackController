#pragma once
#include <string>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <chrono>

class FormattingHelper
{
public:
	FormattingHelper() {};

	// returns a two-digit fixed-width string with a leading-zero if needed. Inverse of parse_number_string(...).
	static std::string format_two_digit_string(int number) {
		std::ostringstream ss;
		ss << std::setw(2) << std::setfill('0') << number;
		return ss.str();
	}
	
	// parses an integer value from a two-digit fixed-width string. Inverse of format_two_digit_string(...).
	static int parse_number_string(std::string number_string) {
		return std::stoi(number_string);
	}

	// https://www.linuxquestions.org/questions/programming-9/convert-a-given-date-to-epoch-time-and-vice-versa-854390/
	static std::string getTimeStamp(time_t epochTime, const char* format = "%Y-%m-%d %H:%M:%S")
	{
		char timestamp[64] = { 0 };
		strftime(timestamp, sizeof(timestamp), format, localtime(&epochTime));
		return timestamp;
	}

	// Converts an unsigned long long value representing the milliseconds since epoch back to a time_point<Clock> (a datetime).
	static std::chrono::time_point<std::chrono::system_clock> date_from_milliseconds_since_epoch(unsigned long long milliseconds_since_epoch);
	// Converts a time_point<Clock> (a datetime) to an unsigned long long value representing the milliseconds since epoch.
	static unsigned long long milliseconds_since_epoch_from_date(std::chrono::time_point<std::chrono::system_clock> datetime);
	static std::string format_datetime(std::chrono::time_point<std::chrono::system_clock> datetime);


};


