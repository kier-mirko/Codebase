#ifndef BASE_TIMER
#define BASE_TIMER

#include <stdio.h>
#include <chrono>

#define TIMER_START()                                                          \
  std::chrono::high_resolution_clock::time_point _time_track_start_ =          \
      std::chrono::high_resolution_clock::now()

#define TIMER_END()                                                            \
  std::chrono::high_resolution_clock::time_point _time_track_end_ =            \
      std::chrono::high_resolution_clock::now();                               \
  auto _time_track_duration_ = _time_track_end_ - _time_track_start_;          \
  printf("%ldms(%ldns)",                                                       \
         std::chrono::duration_cast<std::chrono::milliseconds>(                \
             _time_track_duration_)                                            \
             .count(),                                                         \
         std::chrono::duration_cast<std::chrono::nanoseconds>(                 \
             _time_track_duration_)                                            \
             .count())

#define TIME_TRACK(expr)                                                       \
  _stmt(TIMER_START(); (void *)(expr);                                         \
        std::chrono::high_resolution_clock::time_point _time_track_end_ =      \
            std::chrono::high_resolution_clock::now();                         \
        auto _time_track_duration_ = _time_track_end_ - _time_track_start_;    \
        printf("`%s` took: %ldms(%ldns)\n", #expr,                             \
               std::chrono::duration_cast<std::chrono::milliseconds>(          \
                   _time_track_duration_)                                      \
                   .count(),                                                   \
               std::chrono::duration_cast<std::chrono::nanoseconds>(           \
                   _time_track_duration_)                                      \
                   .count());)

namespace Base {
struct Timer {
  std::chrono::high_resolution_clock::time_point start;

  Timer() : start(std::chrono::high_resolution_clock::now()) {}
  ~Timer() {
    std::chrono::high_resolution_clock::time_point end =
        std::chrono::high_resolution_clock::now();
    auto duration = (end - start);

    printf(
        "Execution took: %ldms(%ldns)\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(),
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
  }
};
} // namespace Base

#endif
