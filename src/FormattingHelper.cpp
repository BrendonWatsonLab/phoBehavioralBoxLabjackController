#include "FormattingHelper.h"
#include <time.h>
typedef std::chrono::system_clock Clock;



std::chrono::time_point<Clock> FormattingHelper::date_from_milliseconds_since_epoch(unsigned long long milliseconds_since_epoch)
{
	std::chrono::milliseconds dur(milliseconds_since_epoch);
	std::chrono::time_point<std::chrono::system_clock> dt(dur);
	return dt;
}

unsigned long long FormattingHelper::milliseconds_since_epoch_from_date(std::chrono::time_point<Clock> datetime)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(datetime.time_since_epoch()).count();
}

std::string FormattingHelper::format_datetime(std::chrono::time_point<Clock> datetime)
{
	time_t time_after_duration = Clock::to_time_t(datetime);
	return getTimeStamp(time_after_duration);
}

