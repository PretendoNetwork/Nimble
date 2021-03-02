#ifndef _DEBUG__H
#define _DEBUG__H

#include "../utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

extern "C" size_t strlen(const char *str);
extern "C" int vsnprintf (char * s, size_t n, const char * format, va_list arg );
extern "C" char * strrchr( const char * string, int searchedChar );

extern int __debug_udp_socket;

namespace WiiU {
	namespace Debugger {

	void Start();
	void Log(const char *txt);
	void Logf(const char *fmt, ...);

	}
}
#define __FILENAME_X__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILENAME_X__)

#define LogFunction(FMT, ARGS...)do { \
    WiiU::Debugger::Logf("%8s: %s\n%8s: %s\n%8s: " FMT "\n", "Filename", __FILENAME__, "Function", __PRETTY_FUNCTION__, "Content", ## ARGS); \
    } while (0)

namespace WiiU {
	namespace Exception {

		void ExceptionHandler(const char *type, OSContext *context);

	}
}
#endif