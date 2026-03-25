#ifndef _DMA_H_
#define _DMA_H_

void dmaInit(void);
void dmaCheckTrans(void);
void dmaRead(void* vAddr, s32 size, u32 devAddr);
void dmaWrite(s32 arg0, u32 arg1, void* arg2);
void dmaWaitBlock(void);
void dmaReadNonBlock(void* arg0, u32 arg1, u32 arg2);
void dmaWriteNonBlock(u32 arg0, u32 arg1, void* arg2);
void dmaStart(void* arg0, u32 arg1, u32 arg2, s32 arg3);

#endif // _DMA_H_
