#ifndef _SCR_H
#define _SCR_H

#ifdef __cplusplus
extern "C" {
#endif

void deinitScreen();
void initScreen();
void printf_(const char *a, u32 val);
void clearScreen();
void flipBuffers();


#ifdef __cplusplus
}
#endif

#endif