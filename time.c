#include <time.h>

#include "time.h"

inline fn bool isLeapYear(u32 year) {
    return (year % 4 == 0 && year % 100 != 0) ||
	   (year % 400 == 0);
}

inline fn DateTime currentDateTime() {
#if OS_LINUX || OS_BSD || OS_MAC
  u64 now = time(0);
  return dateTimeFromUnix(now);
#elif OS_WINDOWS
  // TODO: i don't know whats used by Windows
  Assert(false);
#else
  return (DateTime) {0};
#endif
}

DateTime dateTimeFromUnix(u64 timestamp) {
    DateTime dt = {.year = 1970, .day = 1, .month = 1};

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

u64 unixFromDateTime(DateTime dt) {
  Assert(dt.year >= 1970);
  u64 unix_time = ((dt.day - 1) * UNIX_DAY) +
		  (dt.hour * UNIX_HOUR) +
		  (dt.minute * UNIX_MINUTE) +
		  (dt.second);

  for (u32 year = 1970; year < dt.year; ++year) {
    unix_time += isLeapYear(year) ? UNIX_LEAP_YEAR : UNIX_YEAR;
  }

  for (u8 month = 1; month < dt.month; ++month) {
    unix_time += daysXmonth[month - 1] * UNIX_DAY;
    if (month == 2 && isLeapYear(dt.year)) {
      unix_time += UNIX_DAY;
    }
  }

  return unix_time;
}

// TODO: test the edge cases
DateTime localizeDateTime(DateTime dt, i8 utc_offset) {
  Assert(utc_offset >= -11 && utc_offset <= 14);

  i8 new_hour = dt.hour + utc_offset;
  if (new_hour >= 24) {
    dt.hour = new_hour % 24;
    ++dt.day;
  } else if (new_hour < 0) {
    dt.hour = 24 + new_hour;
    --dt.day;
  } else {
    dt.hour = new_hour;
    return dt;
  }

  if (dt.day > daysXmonth[dt.month - 1]) {
    if (dt.month == 12) {
      dt.month = 1;
      ++dt.year;
    } else {
      ++dt.month;
    }

    dt.day = dt.day % daysXmonth[dt.month - 1];
  } else if (dt.day == 0) {
    if (dt.month == 1) {
      dt.month = 12;
      --dt.year;
    } else {
      --dt.month;
    }

    dt.day = daysXmonth[dt.month - 1];
  }

  return dt;
}
