#include "patcher.h"
#include "IOSU_Exploit.h"

#include "led.h"
#include "binary_data/pretendo_patcher.h"


void Patch_IOSU_URLs(BOOL URLs, BOOL SSL, char *domain)
{

	
	if(URLs == TRUE)
		*(volatile int*)0xF5FFFFFC = 1;

	if(SSL == TRUE)
		*(volatile int*)0xF5FFFFFC = 2;

	if(SSL == TRUE && URLs == TRUE)
		*(volatile int*)0xF5FFFFFC = 3;

	DCFlushRange((int*)0xF5FFFFFC, 4);

	/* Replace ABCD by our domain for each URL */
	for (int i = 0; i < 25; ++i)
	{

		/* find "ABCD" in our custom URL */
		char *s = strstr((const char*)&url_patches[i].url, "ABCD");
		char *end = s+4;
		int r_size = strlen(end) + 1;

		/* save data after "ABCD" */
		char temp_buf[80];
		memcpy(temp_buf, end, r_size);

		/* copy our actual domain and concat the old buffer */
		strcpy(s, domain);
		memcpy(s + strlen(domain), temp_buf, r_size);
	}

	/* Copy our patches into MEM2 */
	*(int*)0x1080B000 = 25;
	memcpy((char*)0x1080C000, (char*)url_patches, sizeof(url_patches));
	DCFlushRange((char*)0x1080B000, sizeof(url_patches) + 0x2000);

	/* Wait before doing the exploit */
	OSSleepTicks(OSMillisecondsToTicks(500));

	/* do the exploit */
	IOSU_Exploit((uint8_t*)pretendo_patcher_bin, pretendo_patcher_bin_len);

	ChangeDrcLedPattern(DRC_LED_TWO_TICKS);

}