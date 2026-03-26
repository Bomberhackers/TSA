#include <ultra64.h>
#include "dma.h"

extern void* D_80095C80;
extern OSMesgQueue D_80095CA0;
extern s32 D_80095CB8;
extern OSMesgQueue D_80095CD8;
extern void* D_80095CF0;
extern OSIoMesg D_80095CC0;

void dmaInit(void) {
    D_80095CB8 = 0;
    osCreatePiManager(0x96, &D_80095CA0, &D_80095C80, 8);
    osCreateMesgQueue(&D_80095CD8, &D_80095CF0, 1);
}

void dmaCheckTrans(void) {
    osRecvMesg(&D_80095CD8, NULL, 0);
}

void dmaRead(void* vAddr, s32 size, u32 devAddr) {
    if (D_80095CB8 != 0) {
        do {
            osYieldThread();
        } while (D_80095CB8 != 0);
    }
    D_80095CB8 = 1;
    osWritebackDCache(vAddr, size);
    osInvalDCache(vAddr, size);
    osInvalICache(vAddr, size);
    osPiStartDma(&D_80095CC0, 0, 0, devAddr, vAddr, (u32) size, &D_80095CD8);
    osRecvMesg(&D_80095CD8, NULL, 1);
    D_80095CB8 = 0;
}

void dmaWrite(s32 arg0, u32 arg1, void* arg2) {
    if (D_80095CB8 != 0) {
        do {
            osYieldThread();
        } while (D_80095CB8 != 0);
    }
    D_80095CB8 = 1;
    osWritebackDCache(arg2, arg0);
    osPiStartDma(&D_80095CC0, 0, 1, (u32) arg0, arg2, arg1, &D_80095CD8);
    osRecvMesg(&D_80095CD8, NULL, 1);
    D_80095CB8 = 0;
}

void dmaWaitBlock(void) {
    osRecvMesg(&D_80095CD8, NULL, 1);
}

void dmaReadNonBlock(void* arg0, u32 arg1, u32 arg2) {
    osPiStartDma(&D_80095CC0, 0, 0, arg2, arg0, arg1, &D_80095CD8);
}

void dmaWriteNonBlock(u32 arg0, u32 arg1, void* arg2) {
    osPiStartDma(&D_80095CC0, 0, 1, arg0, arg2, arg1, &D_80095CD8);
}

void dmaStart(void* arg0, u32 arg1, u32 arg2, s32 arg3) {
    osPiStartDma(&D_80095CC0, 0, arg3, arg2, arg0, arg1, &D_80095CD8);
}
