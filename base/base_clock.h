#ifndef BASE_TIME_H
#define BASE_TIME_H

typedef struct DateTime DateTime;
struct DateTime{
  I32 year;
  U8 month;
  U8 day;
  U8 hour;
  U8 minute;
  U8 second;
  U8 ms;
};

#define UNIX_MINUTE 60
#define UNIX_HOUR 3600
#define UNIX_DAY 86400
#define UNIX_WEEK 604800

#define UNIX_YEAR 31536000
#define UNIX_LEAP_YEAR 31622400

global const U8 daysXmonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline B32 t_is_leap_year(U32 year);

inline DateTime t_current_date_time();
inline DateTime t_local_current_date_time(I8 utc_offset);

// TODO: maybe add more time formats?
DateTime t_date_time_from_unix(U64 timestamp);
U64 t_unix_from_date_time(DateTime dt);

DateTime t_localize_date_time(DateTime dt, I8 utc_offset);

#endif
