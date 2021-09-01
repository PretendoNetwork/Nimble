#ifndef _UTLS_H
#define _UTLS_H

#define MK8_J 0x000500001010EB00
#define MK8_U 0x000500001010EC00
#define MK8_E 0x000500001010ED00

#define PROJECT_NAME			"Full Keys Dumper (OTP, BOSS, IDBE)"
#define PROJECT_VER_MAJOR		1
#define PROJECT_VER_VERSION		0
#define PROJECT_VER_MINOR		0
#define PROJECT_VER_DISC 		"release"
#define PROJECT_AUTHOR			"Rambo6Glaz, QuarkTheAwesome and the Pretendo Team"

#define PROJECT_PRIV_ALPHA		0
#define PROJECT_PRIV_BETA		1
#define PROJECT_BIN_VER			((PROJECT_PRIV_ALPHA << 28) | (PROJECT_PRIV_BETA << 24) | (PROJECT_VER_MAJOR << 16) | (PROJECT_VER_VERSION << 8) | PROJECT_VER_MINOR)

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>

#include "wiiu/symbols.h"

#define C_UNLESS(expr, code) \
    ({ \
        if (!(expr)) { \
            code; \
        } \
    })

#define R_UNLESS(expr, res) \
    ({ \
        if (!(expr)) { \
            return res; \
        } \
    })


#define FIND_ENTRY(arr, max_c, value) \
		auto ret_idx = max_c; \
		for(auto i = 0; i < max_c; i++) {	\
			if(arr[i] == value) { \
				ret_idx = i; \
				break; \
			} \
		} \

#define FIND_ENTRY_MEMBER(arr, max_c, member, value) \
		auto ret_idx = max_c; \
		for(auto i = 0; i < max_c; i++) {	\
			if(arr[i]. ## member  == value) { \
				ret_idx = i; \
				break; \
			} \
		} \

#define FIND_ENTRY_MEMBER_(arr, max_c, member, value) \
		auto ret_idx = max_c; \
		for(auto i = 0; i < max_c; i++) {	\
			if(arr[i]-> ## member  == value) { \
				ret_idx = i; \
				break; \
			} \
		} \

#define FIND_ENTRY_MEMBER_EX(arr, max_c, member, value, code) \
		for(auto i = 0; i < max_c; i++) {	\
			if(arr[i].member  == value) { \
				code; \
				break; \
			} \
		} \

#define FIND_ENTRY_MEMBER_EX_(arr, max_c, member, value, code) \
		for(auto i = 0; i < max_c; i++) {	\
			if(arr[i]-> ## member  == value) { \
				code;\
				break; \
			} \
		} \


#define RETURN_TO_HBL 			(0)
#define RETURN_TO_NEXT_APP 		(-3)

#endif
