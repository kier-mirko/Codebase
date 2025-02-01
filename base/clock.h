#ifndef BASE_CLOCK_H
#define BASE_CLOCK_H

#include <time.h>

typedef struct {
  i32 year;
  u8 month;
  u8 day;
  u8 hour;
  u8 minute;
  u8 second;
  u16 ms;
} DateTime;

typedef u64 time64;

#define UNIX_MINUTE 60
#define UNIX_HOUR 3600
#define UNIX_DAY 86400
#define UNIX_WEEK 604800

#define UNIX_YEAR 31536000
#define UNIX_LEAP_YEAR 31622400

global const u8 daysXmonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline fn bool isLeapYear(u32 year);

// =============================================================================
// Time format conversions
fn DateTime dateTimeFromTime64(time64 timestamp);
fn time64 time64FromDateTime(DateTime *dt);
fn time64 time64FromUnix(u64 timestamp);

fn DateTime dateTimeFromUnix(u64 timestamp);
fn u64 unixFromDateTime(DateTime *dt);
fn u64 unixFromTime64(time64 timestamp);

#endif // BASE_CLOCK_H
