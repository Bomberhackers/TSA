#ifndef _FBLOCK_H_
#define _FBLOCK_H_

void fexecLoad(s32 arg0);
u32 fexecLoadAddress(s32 id, u32 (*func)());
s32 fexecGetFileSize(s32 id);
void fexecCall(void);
void fexecInit(void);
void fexecChangeBlock(s32 arg0);

#endif // _FBLOCK_H_
