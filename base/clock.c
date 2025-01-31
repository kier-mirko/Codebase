inline fn bool isLeapYear(u32 year) {
    return (year % 4 == 0 && year % 100 != 0) ||
	   (year % 400 == 0);
}

inline fn DateTime localCurrentDateTime(i8 utc_offset) {
  return localizeDateTime(os_currentDateTime(), utc_offset);
}

DateTime dateTimeFromUnix(u64 timestamp) {
  DateTime dt = {.year = 1970, .month = 1, .day = 1};

  for (u64 secondsXyear = isLeapYear(dt.year)
			  ? UNIX_LEAP_YEAR
			  : UNIX_YEAR;
       timestamp >= secondsXyear;
       ++dt.year, timestamp -= secondsXyear,
       secondsXyear = isLeapYear(dt.year)
		      ? UNIX_LEAP_YEAR
		      : UNIX_YEAR);

  while (1) {
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

  dt.day += (u8)(timestamp / UNIX_DAY);
  timestamp %= UNIX_DAY;

  dt.hour = (u8)(timestamp / UNIX_HOUR);
  timestamp %= UNIX_HOUR;

  dt.minute = (u8)(timestamp / UNIX_MINUTE);
  timestamp %= UNIX_MINUTE;

  dt.second = (u8)timestamp;

  return dt;
}

u64 unixFromDateTime(DateTime dt) {
  Assert(dt.year >= 1970);
  u64 unix_time = ((dt.day - 1) * UNIX_DAY) +
		  (dt.hour * UNIX_HOUR) +
		  (dt.minute * UNIX_MINUTE) +
		  (dt.second);

  for (u32 year = 1970; year < (u32)dt.year; ++year) {
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

fn DenseTime denseTimeFromDateTime(DateTime *date_time){
  DenseTime result = 0;
  
  result += date_time->year;
  result *= 12;
  result += date_time->month;
  result *= 31;
  result += date_time->day;
  result *= 24;
  result += date_time->hour;
  result *= 60;
  result += date_time->minute;
  result *= 60;
  result += date_time->second;
  result *= 1000;
  result += date_time->ms;
  
  return result;
}

fn DateTime dateTimeFromDenseTime(DenseTime dense_time){
  DateTime result = {0};
  
  result.ms = dense_time % 1000;
  dense_time /= 1000;
  result.second = dense_time % 60;
  dense_time /= 60;
  result.minute = dense_time % 60;
  dense_time /= 60;
  result.hour = dense_time % 24;
  dense_time /= 24;
  result.day = dense_time % 31;
  dense_time /= 31;
  result.month = dense_time % 12;
  dense_time /= 12;
  result.year = (u32)dense_time;
  
  return result;
}
