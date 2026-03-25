#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "ultra64.h"

extern void idle(void*);                                   /* extern */
extern s32 memset(void*, s32, s32);                               /* extern */
extern s32 resetwatch();                                     /* extern */
extern s32 AddSchedulerClient(s32*, OSMesgQueue*, s32);          /* extern */
extern s32 CalcVecSet();                                     /* extern */
extern s32 CallProcess(s32);                                   /* extern */
extern s32 FlushController();                    /* extern */
extern s32 InitController(s32, s32);                             /* extern */
extern s32 InitExController();                               /* extern */
extern s32 InitMalloc(s32*, s32);                                /* extern */
extern s32 InitPack();                                       /* extern */
extern s32 InitProcess();                                    /* extern */
extern s32 InitSI();                                         /* extern */
extern s32 InitScheduler(s32, s32);                              /* extern */
extern s32 InitVibrator();                                   /* extern */
extern s32 SetAutoReadController();                          /* extern */
extern s32 UpdateExController();                             /* extern */
extern s32 camInit();                                        /* extern */
extern void* dpmBuild(s32, s32, s32);                          /* extern */
extern s32 dpmInit();                                        /* extern */
extern s32 dpmReset();                                       /* extern */
extern s32 dpmSortTable();                                   /* extern */
extern s32 fadeExec(void*);                                /* extern */
extern s32 fadeFadein2(s32, s32, s32, s32);                          /* extern */
extern s32 fadeInit();                                       /* extern */
extern s32 fexecInit();                                      /* extern */
extern s32 gameresInit();                                    /* extern */
extern s32 gfxBegin(s32);                                    /* extern */
extern s32 gfxEnd(s32, s32);                                 /* extern */
extern s32 gfxGetPointer(s32);                             /* extern */
extern s32 gfxInit();                                        /* extern */
extern s32 gfxInitDisplayLimit(s32);                       /* extern */
extern s32 gfxSetPointer(s32, void*);                        /* extern */
extern s32 initAudio();                                      /* extern */
extern s32 init_proc();                                      /* extern */
extern s32 init_rnd(u32);                                    /* extern */
extern s32 malloc(s32);                                      /* extern */
extern s32 ndBuildSceneGfx(s32, s32);                      /* extern */
extern s32 ndRealFreeEmbedB();                               /* extern */
extern s32 obj2APIInit();                                    /* extern */
extern s32 obj2Draw();                                       /* extern */
extern s32 obj2Init(s32, s32);                                 /* extern */
extern s32 obj2Pass1();                                      /* extern */
extern s32 obj2Pass2();                                      /* extern */
extern s32 osViSetSpecialFeature(s32);                         /* extern */
extern s32 preNMIInit();                                     /* extern */
extern s32 preNMIexec(s32);                                /* extern */
extern s32 resetwatch();                                     /* extern */
extern s32 rumbleInit();                                     /* extern */
extern s32 soundEntryDrive();                                /* extern */
extern s32 sprInit();                                        /* extern */
extern s32 viewDraw(s32);                                  /* extern */
extern void* viewDrawSprite(s32, s32*);                    /* extern */
extern s32 viewInit();                                       /* extern */
extern s32 zerojumpinit();                                   /* extern */
extern s32 zjSetVec(s32, s32*);                                  /* extern */
extern s32 znzInit();                                        /* extern */
extern s32 dmaWrite(s32, s32, void*);                       /* extern */
extern s32 dmaRead(s32, s32, s32);

#endif // _FUNCTIONS_H_
