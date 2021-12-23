#pragma once

#ifdef __WIIU__
#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>

#include <whb/log.h>

#endif
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __FILENAME_X__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILENAME_X__)

#ifdef __WIIU__
    #define DEBUG_FUNCTION_LINE(FMT, ARGS...)do { \
        WHBLogPrintf("[%23s]%30s@L%04d: " FMT "",__FILENAME__,__FUNCTION__, __LINE__, ## ARGS); \
        } while (0)
#else
    #define DEBUG_FUNCTION_LINE(FMT, ARGS...)do { \
            printf("[%23s]%30s@L%04d: " FMT "\n",__FILENAME__,__FUNCTION__, __LINE__, ## ARGS); \
            } while (0)
#endif
#ifdef __cplusplus
}
#endif

