#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nsysnet/socket.h>
#include <coreinit/memory.h>
#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <coreinit/screen.h>
#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memory.h>
#include <coreinit/ios.h>
#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <coreinit/context.h>
#include <coreinit/exception.h>
#include <coreinit/internal.h>
#include <coreinit/core.h>
#include <coreinit/title.h>
#include <coreinit/debug.h>
#include <coreinit/memorymap.h>
#include <coreinit/messagequeue.h>

#include <curl/curl.h>
#include <nsysnet/socket.h>
#include <nsysnet/nssl.h>

#include "network.h"

int write_data(void *buffer, int size, int nmemb, void *userp)
{
	uint32_t *data = (uint32_t*)userp;

	int filepos = data[1];
	int insize = size*nmemb;

	memcpy((char*)(data[0]+filepos), buffer, insize);
	DCFlushRange((char*)(data[0]+filepos), insize);
	data[1] += insize;

	return insize;

}

int NetworkThreadStart(int argc, const char **argv)
{

	uint32_t *yeet = (uint32_t*)argv;
	char *url = (char*)yeet[1];
	uint32_t *where_to = (uint32_t*)yeet[0];
	int *done = (int*)yeet[2];

	socket_lib_init();
	NSSLInit();
	CURL * curl = curl_easy_init();

	uint32_t *data = (uint32_t*)OSAllocFromSystem(8, 4);
	data[0] = (uint32_t)OSAllocFromSystem(0x1000, 4);
	data[1] = 0;

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)data);

	NSSLContextHandle ssl_context = NSSLCreateContext(0);

	for(int i = 100; i<106; i++)
		NSSLAddServerPKI(ssl_context, (NSSLServerCertId)i);

	for(int i = 1001; i<1034; i++)
		NSSLAddServerPKI(ssl_context, (NSSLServerCertId)i);
	
	curl_easy_setopt(curl, CURLOPT_NSSL_CONTEXT, ssl_context);
	curl_easy_perform(curl);

	NSSLDestroyContext(ssl_context);
	curl_easy_cleanup(curl);

	int resp = -1;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp);
	WHBLogPrintf("Response code: %d", resp);

	memcpy(where_to, (char*)data[0], 0x1000);
	DCFlushRange(where_to, 0x1000);

	*done = resp;

	/* You should NEVER EVER DO THAT */
	OSFreeToSystem(argv);
	OSFreeToSystem((void*)data[0]);
	OSFreeToSystem(data);
	OSFreeToSystem(OSGetCurrentThread()->stackStart);
	OSFreeToSystem(OSGetCurrentThread());

	OSExitThread(1);
	return 0;
}

void DownloadFile(uint8_t *ptr_to_write_once_done, const char *url, int *done)
{

	OSThread *NetworkThread = (OSThread*)OSAllocFromSystem(sizeof(OSThread), 16);
	uint8_t *NetworkThreadStack = (uint8_t*)OSAllocFromSystem(0x2000, 16);

	uint32_t *args = (uint32_t*)OSAllocFromSystem(12, 4);
	args[0] = (uint32_t)ptr_to_write_once_done;
	args[1] = (uint32_t)url;
	args[2] = (uint32_t)done;
	DCFlushRange(args, 8);

	OSCreateThread(NetworkThread, NetworkThreadStart, 1, (char*)args, (void*)(NetworkThreadStack + 0x2000), 0x2000, 0, OSGetThreadAffinity(OSGetCurrentThread()));
	OSResumeThread(NetworkThread);

}
