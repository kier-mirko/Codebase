#ifndef BASE_TIMER
#define BASE_TIMER

#include <stdio.h>
#include <time.h>

#define TimedScope                                                             \
  for (clock_t ___time_track_start = clock(), ___time_track_end,               \
               ___timer_once = 1;                                              \
       ___timer_once; ___timer_once = 0, ___time_track_end = clock(),          \
               printf("%ldms (%ldns)\n",                                       \
                      (u64)(((f64)(___time_track_end - ___time_track_start)) * \
                            1000 / CLOCKS_PER_SEC),                            \
                      (u64)(((f64)(___time_track_end - ___time_track_start)) * \
                            1e9 / CLOCKS_PER_SEC)))

#define TimeTrack(expr)                                                        \
  _stmt(for (clock_t ___time_track_start = clock(), ___time_track_end,         \
             ___timer_once = 1;                                                \
             ___timer_once;                                                    \
             ___timer_once = 0, ___time_track_end = clock(),                   \
             printf("`%s` took %ldms (%ldns)\n", #expr,                        \
                    (u64)(((f64)(___time_track_end - ___time_track_start)) *   \
                          1000 / CLOCKS_PER_SEC),                              \
                    (u64)(((f64)(___time_track_end - ___time_track_start)) *   \
                          1e9 / CLOCKS_PER_SEC))) { (void)(expr); })

#endif
