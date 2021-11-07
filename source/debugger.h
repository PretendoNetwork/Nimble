#ifndef _DEBUG__H
#define _DEBUG__H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <malloc.h>

#include "symbols.h"

void Debugger_Start();
void Debugger_Log(const char *txt);
void Debugger_Logf(const char *fmt, ...);

#define __FILENAME_X__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILENAME_X__)

#define LogFunction(FMT, ARGS...)do { \
    WiiU::Debugger::Logf("%8s: %s\n%8s: %s\n%8s: " FMT "\n", "Filename", __FILENAME__, "Function", __PRETTY_FUNCTION__, "Content", ## ARGS); \
    } while (0)

void Exception_ExceptionHandler(const char *type, OSContext *context);

#endif