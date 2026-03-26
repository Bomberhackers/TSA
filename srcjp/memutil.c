#include <ultra64.h>
#include "memutil.h"

u8 *hmemset(u8 *dst, u32 c, s32 size) {
    u8 *new_dst = dst;

    while (size--) {
        *(new_dst++) = c;
    }

    return dst;
}

void *hmemmove(void *dest, void *src, size_t len) {
    u8 *destp = dest;
    u8 *srcp = src;

    if ((len != 0) && (dest != src)) {
        if ((srcp < destp) && (destp < srcp + len)) {
            while (len--) {
                *(len + destp) = *(len + srcp);
            }
        }
        else {
            while (len--) {
                *destp++ = *srcp++;
            }
        }
    }

    return dest;
}

// officially named strcat but this may cause a conflict with libultra's function.
// I am not sure how Hudson got around this.
u8 *hstrcat(u8 *str, u8 *str1) {
    u8 *ret = str;

    while (*str != 0) {
        str++;
    }

    while (*(str++) = *(str1++))
        ;

    return ret;
}
