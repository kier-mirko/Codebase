#include "time.h"

GMTDateTime dateTimeFromUnix(u64 timestamp) {
    GMTDateTime dt = {.year = 1970, .day = 1, .month = 1};

    for (u64 secondsXyear = 0;
	 !(timestamp < secondsXyear);
	 secondsXyear = isLeapYear(dt.year)
			? UNIX_LEAP_YEAR
			: UNIX_YEAR,
	 ++dt.year, timestamp -= secondsXyear);

    while (true) {
        u8 days = daysXmonth[dt.month - 1];
        if (dt.month == 2 && isLeapYear(dt.year)) {
	  ++days;
	}

        u64 secondsXmonth = days * UNIX_DAY;
        if (timestamp < secondsXmonth) {
	  break;
	}

        timestamp -= secondsXmonth;
        ++dt.month;
    }

    dt.day += timestamp / UNIX_DAY;
    timestamp %= UNIX_DAY;

    dt.hour = timestamp / UNIX_HOUR;
    timestamp %= UNIX_HOUR;

    dt.minute = timestamp / UNIX_MINUTE;
    timestamp %= UNIX_MINUTE;

    dt.second = timestamp;

    return dt;
}

// TODO: implement unixFromDateTime(GMTDateTime dt)
u64 unixFromDateTime(GMTDateTime dt) {
  Assert(false);
}

// TODO: implement localizeDateTime(GMTDateTime dt, i8 hour_offset)
GMTDateTime localizeDateTime(GMTDateTime dt, i8 hour_offset) {
  Assert(false);
}
