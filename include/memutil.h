#ifndef _MEMUTIL_H_
#define _MEMUTIL_H_

u8 *hmemset(u8 *dst, u32 c, s32 size);
void *hmemmove(void *dest, void *src, size_t len);
u8 *hstrcat(u8 *str, u8 *str1);

#endif // _MEMUTIL_H_
