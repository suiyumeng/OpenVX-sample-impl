#include <sys/time.h>
#include <ctime>

#define TIME_SLOTS 10
thread_local unsigned long long _time_begin[TIME_SLOTS];
thread_local unsigned long long _time_end[TIME_SLOTS];
static unsigned long long GetTime(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (unsigned long long)(time.tv_usec + time.tv_sec * 1000000);
}

void time_begin(int id) { _time_begin[id] = GetTime(); }

void time_end(int id) { _time_end[id] = GetTime(); }

unsigned long long time_consumed(int id) { return _time_end[id] - _time_begin[id]; }