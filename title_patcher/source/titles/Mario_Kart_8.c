#include "Mario_Kart_8.h"

typedef struct sead_String {
    char* data;
    uint32_t vtable;
} sead_String;

void (*real_enl_PiaUtil_ParseIdentificationToken)(void* identificationInfo, sead_String* identificationToken);
void hook_enl_PiaUtil_ParseIdentificationToken(void* identificationInfo, sead_String* identificationToken) {
    if (strnlen(identificationToken->data, 16) == 16) {
        identificationToken->data[15] = 0;
    }
    real_enl_PiaUtil_ParseIdentificationToken(identificationInfo, identificationToken);
}

void Patch_Mario_Kart_8(uint32_t titleVer, uint64_t titleId) {
    printf("MK8 patch is temporarily disabled pending debugging\n");
    //HOOK_FUNC(0x028E3930 + 0x0C180000, enl_PiaUtil_ParseIdentificationToken);
}
