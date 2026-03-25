#include <ultra64.h>
#include "stdarg.h"
#include "memutil.h"

void systemmain(void);
void mainobj2mode(s32 arg0);
void boot(void);
void idle(void* arg);
void osWriteHost(void* arg0, u32 arg1);
void osReadHost(void* arg0, u32 arg1);
void osCreateLog(OSLog* log, u32* base, s32 len);
void osLogEvent(OSLog *log, s16 code, s16 numArgs, ...);
void osFlushLog(OSLog* log);
u32 osLogFloat(f32 arg0);
void errstop(const char *fmt, ...);
void osSyncPrintf(const char *fmt, ...);

void osSyncPrintf(const char *fmt, ...) {

}

void errstop(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    osSyncPrintf(fmt);

    while(1)
        ;

    va_end(args);
}

u32 osLogFloat(f32 arg0) {
    return 0;
}

void osFlushLog(OSLog* log) {

}

void osLogEvent(OSLog *log, s16 code, s16 numArgs, ...) {

}

void osCreateLog(OSLog* log, u32* base, s32 len) {

}

void osReadHost(void* arg0, u32 arg1) {

}

void osWriteHost(void* arg0, u32 arg1) {

}

void idle(void* arg) {
    dmaInit();
    osCreateThread(&D_80099EC0, 3, systemmain, arg, &D_8009E070, 2);
    osStartThread(&D_80099EC0);
    osSetThreadPri(NULL, 0);

    while(1)
        ;
}

void boot(void) {
    resetwatch();
    osUnmapTLBAll();
    // TODO: This is clearing some .data/.rodata area, not specifically this symbol
    hmemset(gSecureCallArr, 0, (u8*)D_800AE350 - (u8*)gSecureCallArr);
    hmemset(D_800B2350, 0, D_800DBCA0 - D_800B2350);
    hmemset(0x80250000, 0, 0x80000);
    osWritebackDCacheAll();
    osInitialize();
    if (D_80000300 == 0) {
        while(1)
            ;
    }
    osCreateThread(&D_80095D10, 1, idle, NULL, &D_80099EC0, 0xA);
    osStartThread(&D_80095D10);
}

void mainobj2mode(s32 arg0) {
    D_8008C6A8 = arg0;
}

void systemmain(void) {
    Gfx *gfx;
    u32 size;
    s32 sp94;
    OSMesgQueue sp7C;
    s32 pad[7];
    void* sp5C;
    s32 sp58;
    s32 sp54;
    s32 temp_s0;
    s32 temp_v0;
    void* temp_v0_2;
    s32 sp44;
    s32 sp40;
    void* temp_v0_3;

    sp54 = 0;
    size = 0x41000;
    zerojumpinit();
    zjSetVec(1, &D_8008C6D4);
    zjSetVec(0x10, &D_8008C6F8);
    zjSetVec(0x11, &D_8008C744);
    zjSetVec(0x18, &D_8008C768);
    resetwatch();
    InitMalloc(&D_802D0000, 0x130000);
    if (D_80000300 != 1) {
        osSetThreadPri(NULL, 0);
        while(1)
            ;
    } else {
        InitScheduler(2, 1);
    }
    InitSI();
    preNMIInit();
    InitController(4, 0);
    InitPack();
    InitVibrator();
    InitProcess();
    osCreateMesgQueue(&sp7C, &sp5C, 8);
    AddSchedulerClient(&sp94, &sp7C, 1);
    InitExController();
    zjSetVec(2, &D_8008C6AC);
    zjSetVec(3, &D_8008C6CC);
    osViSetSpecialFeature(2);
    osViSetSpecialFeature(4);
    osViSetSpecialFeature(0x40);
    osViSetSpecialFeature(0x10);
    CalcVecSet();
    SetAutoReadController();
    osRecvMesg(&sp7C, (OSMesg*)&sp58, 1);
    osRecvMesg(&sp7C, (OSMesg*)&sp58, 1);
    fexecInit();
    initAudio();
    dpmInit();
    obj2Init(malloc(size), size);
    obj2APIInit();
    znzInit();
    sprInit();
    viewInit();
    gfxInit();
    gameresInit();
    rumbleInit();
    fadeInit();
    init_rnd(osGetTime());
    camInit();
    init_proc();
    fadeFadein2(0, 0, 0, 0x3C);
    if ((D_8009E2F0 != 0) || (D_80091C9C != 0)) {
        while(1)
            ;
    }

    while (1) {
        osRecvMesg(&sp7C, (OSMesg*)&sp58, 1);
        if (sp58 != 1) {
            continue;
        }
        if ((D_80095D04 - sp54) < 2) {
            continue;
        }
        sp54 = D_80095D04;
        FlushController();
        UpdateExController();
        if (D_8009E2F0 == 0) {
            CallProcess(1);
        } else {
            D_8008C6A8 = 0;
        }
        if (D_8008C6A8 != 0) {
            obj2Pass1();
        }
        soundEntryDrive();
        temp_s0 = gfxBegin(0x3E8);
        if (temp_s0 == 0) {
            continue;
        }
        D_8008C6A4 += 1;
        dpmReset();
        temp_s0 = gfxInitDisplayLimit(temp_s0);
        obj2Pass2();
        obj2Draw();
        sp44 = gfxGetPointer(D_8009E098);
        dpmSortTable();
        gfx = dpmBuild(ndBuildSceneGfx(sp44, D_800A41F0), 0x1F, 0);
        gSPEndDisplayList(gfx++);
        gfxSetPointer(D_8009E098, gfx);
        gfx = viewDrawSprite(viewDraw(temp_s0), &sp40);
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, 0, 0, 320, 240);
        gfxEnd(preNMIexec(fadeExec(gfx)), sp40);
        ndRealFreeEmbedB();
        D_8008C6A0 += 1;
    }
}
