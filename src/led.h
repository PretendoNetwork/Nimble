#ifndef __LED_H
#define __LED_H

#include <coreinit/dynload.h>

typedef enum DrcLedPattern
{
    DRC_LED_OFF = 0, // DRC LED deactivated
    DRC_LED_ON = 1,  // DRC LED always activated
    DRC_LED_TWO_TICKS = 2, // DRC LED ticking twice rapidly (Friend List Notifications)
    DRC_LED_ONE_TICK = 3, // DRC LED ticking once briefly
    DRC_LED_THREE_TICKS = 4, // DRC LED ticking three times infinitely with a pause (WiiU Chat notifications)

} DrcLedPattern;

void InitDRCStuff();
void ChangeDrcLedPattern(DrcLedPattern pattern);

#endif