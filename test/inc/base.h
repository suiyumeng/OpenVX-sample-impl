#ifndef _BASE__
#define _BASE__

#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sys/time.h>
#include <ctime>
#include <string.h>
#include "osal.h"

#ifndef dimof
/*! \brief A helper macro to determine the number of elements in an array.
 * \ingroup group_helper
 */
#define dimof(x)    (sizeof(x)/sizeof(x[0]))
#endif

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

    #define DEBUG_ON
    using namespace std;

    static struct timeval tv;
#define LOG_E(format, ...)                                   \
    gettimeofday(&tv, NULL);                                 \
    do                                                       \
    {                                                        \
        fprintf(stdout,                                      \
                "[ERRR] %u %u %lu:%lu %s:%d " format, \
                osal_getpid(),                               \
                osal_gettid(),                               \
                tv.tv_sec,                                   \
                (unsigned long)tv.tv_usec,                                  \
                __func__,                                    \
                __LINE__,                                    \
                ##__VA_ARGS__);                              \
    } while (0);

#define LOG_W(format, ...)                                   \
    gettimeofday(&tv, NULL);                                 \
    do                                                       \
    {                                                        \
        fprintf(stdout,                                      \
                "[WARN] %u %u %lu:%lu %s:%d " format, \
                osal_getpid(),                               \
                osal_gettid(),                               \
                tv.tv_sec,                                   \
                (unsigned long)tv.tv_usec,                                  \
                __func__,                                    \
                __LINE__,                                    \
                ##__VA_ARGS__);                              \
    } while (0);

#define LOG_I(format, ...)                                   \
    gettimeofday(&tv, NULL);                                 \
    do                                                       \
    {                                                        \
        fprintf(stdout,                                      \
                "[INFO] %u %u %lu:%lu %s:%d " format, \
                osal_getpid(),                               \
                osal_gettid(),                               \
                tv.tv_sec,                                   \
                (unsigned long)tv.tv_usec,                                  \
                __func__,                                    \
                __LINE__,                                    \
                ##__VA_ARGS__);                              \
    } while (0);

#ifdef DEBUG_ON
#define LOG_D(format, ...)                                   \
    gettimeofday(&tv, NULL);                                 \
    do                                                       \
    {                                                        \
        fprintf(stdout,                                      \
                "[DBUG] %u %u %lu:%lu %s:%d " format, \
                osal_getpid(),                               \
                osal_gettid(),                               \
                tv.tv_sec,                                   \
                (unsigned long)tv.tv_usec,                                  \
                __func__,                                    \
                __LINE__,                                    \
                ##__VA_ARGS__);                              \
    } while (0);
#else
#define LOG_D(format, ...)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus*/

void time_begin(int id);
void time_end(int id);
unsigned long long time_consumed(int id);

#endif