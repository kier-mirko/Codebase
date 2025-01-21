#ifndef BASE_TIME_H
#define BASE_TIME_H

#include <time.h>

typedef struct {
    i32 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8 ms;
} DateTime;

#define UNIX_MINUTE 60
#define UNIX_HOUR 3600
#define UNIX_DAY 86400
#define UNIX_WEEK 604800

#define UNIX_YEAR 31536000
#define UNIX_LEAP_YEAR 31622400

global const u8 daysXmonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline fn bool isLeapYear(u32 year);

inline fn DateTime currentDateTime();
inline fn DateTime localCurrentDateTime(i8 utc_offset);

// TODO: maybe add more time formats?
DateTime dateTimeFromUnix(u64 timestamp);
u64 unixFromDateTime(DateTime dt);

DateTime localizeDateTime(DateTime dt, i8 utc_offset);

#endif
