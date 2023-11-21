#pragma once



#ifdef TALOS_USE_SERIAL

#else
    #include <stdio.h>
    #define TS_INFO(message)    printf("INFO  %s(L: %d) %s", __PRETTY_FUNCTION__, __LINE__, message)
    #define TS_WARN(message)    printf("WARN  %s(L: %d) %s", __PRETTY_FUNCTION__, __LINE__, message)
    #define TS_ERROR(message)   printf("ERROR %s(L: %d) %s", __PRETTY_FUNCTION__, __LINE__, message)
    #define TS_FATAL(message)   assert(false && message)
    #define TS_INFOF(format, ...)    printf("INFO  %s(L: %d) " format, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define TS_WARNF(format, ...)    printf("WARN  %s(L: %d) " format, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
    #define TS_ERRORF(format, ...)   printf("ERROR %s(L: %d) " format, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#endif