#ifndef BASE_TIMER
#define BASE_TIMER

#include <stdio.h>
#include <time.h>

#define TimedScope							\
  for (struct timespec ___start_time = {0}, ___end_time = {0};		\
       ___start_time.tv_sec == 0 &&					\
	 clock_gettime(CLOCK_MONOTONIC, &___start_time) == 0;		\
       clock_gettime(CLOCK_MONOTONIC, &___end_time),			\
	 printf("%ldms (%ldns)\n",					\
		(u64)(((___end_time.tv_sec - ___start_time.tv_sec) * 1000) + \
		      ((___end_time.tv_nsec - ___start_time.tv_nsec) / 1e6)), \
		(u64)((___end_time.tv_sec - ___start_time.tv_sec) * 1e9 + \
		      (___end_time.tv_nsec - ___start_time.tv_nsec))))	\

#define TimeTrack(expr) _stmt(TimedScope { (void)(expr); })

#endif
