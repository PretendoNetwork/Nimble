#include "led.h"

int32_t (*ACPInitialize)(void);
int32_t (*ACPTurnOnDrcLed)(int32_t persistentId, DrcLedPattern pattern);
int32_t (*GetPersistentId__Q2_2nn3actFv)(void);

int32_t pId = -1;

void InitDRCStuff()
{

	OSDynLoad_Module acp_handle, act_handle;
	OSDynLoad_Acquire("nn_acp.rpl", &acp_handle);
	OSDynLoad_Acquire("nn_act.rpl", &act_handle);

	OSDynLoad_FindExport(acp_handle, 0, "ACPInitialize", (void**)&ACPInitialize);
	OSDynLoad_FindExport(acp_handle, 0, "ACPTurnOnDrcLed", (void**)&ACPTurnOnDrcLed);
	OSDynLoad_FindExport(act_handle, 0, "GetPersistentId__Q2_2nn3actFv", (void**)&GetPersistentId__Q2_2nn3actFv);

	ACPInitialize();
	pId = GetPersistentId__Q2_2nn3actFv();

}

void ChangeDrcLedPattern(DrcLedPattern pattern)
{
	ACPTurnOnDrcLed(pId, pattern);
}