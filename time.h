#ifndef BASE_TIME_H
#define BASE_TIME_H

#include "base.h"

typedef struct {
    i32 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8 ms;
} GMTDateTime;

#define UNIX_MINUTE 60
#define UNIX_HOUR 3600
#define UNIX_DAY 86400
#define UNIX_WEEK 604800

#define UNIX_YEAR 31536000
#define UNIX_LEAP_YEAR 31622400

global const u8 daysXmonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline fn bool isLeapYear(u32 year) {
    return (year % 4 == 0 && year % 100 != 0) ||
	   (year % 400 == 0);
}

// TODO: maybe add more time formats?
GMTDateTime dateTimeFromUnix(u64 timestamp);
u64 unixFromDateTime(GMTDateTime dt);

GMTDateTime localizeDateTime(GMTDateTime dt, i8 hour_offset);

#endif
