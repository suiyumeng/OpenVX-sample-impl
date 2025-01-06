#ifndef __OSAL__
#define __OSAL__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define     __PRINT_MACRO(x)    #x
#define     PRINT_MACRO(x)      #x "=" __PRINT_MACRO(x)

#ifdef __linux__
#include <sys/syscall.h>

    static inline unsigned int osal_getpid()
    {
        return getpid();
    }
    static inline unsigned int osal_gettid()
    {
        return syscall(SYS_gettid);
    }

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>

    static inline unsigned int osal_getpid()
    {
        return GetCurrentProcessId();
    }
    static inline unsigned int osal_gettid()
    {
        return GetCurrentThreadId();
    }
#elif  defined(__APPLE__)
#include <pthread.h>
#include <sys/syscall.h>
#include <AvailabilityMacros.h>
    static inline unsigned int osal_getpid()
    {
        return getpid();
    }
    static inline unsigned int osal_gettid()
    {
//#pragma message(PRINT_MACRO(MAC_OS_X_VERSION_MAX_ALLOWED))
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
        return pthread_mach_thread_np(pthread_self());
#else
        return syscall(SYS_gettid);
#endif
    }

#endif

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif