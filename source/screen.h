#ifndef _SCREEN_WIIU_H
#define _SCREEN_WIIU_H

#include "symbols.h"
#include <malloc.h>

#define rgba(r, g, b, a) ((uint32_t)((r << 24) | (g << 16) | (b << 8) | a))

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "screen.h"

void Screen_Initialize();
void Screen_Destroy();
void Screen_DrawText(const char *text);
void Screen_DrawTextLine(int line, const char *text);
void Screen_DrawTextf(const char *text, ...);
void Screen_FlushBuffers();
void Screen_StartFrame();
void Screen_EndFrame();

#endif