#include <ultra64.h>
#include "decode.h"

extern s32 D_8008C7B0;
extern u8* D_8009E0B0;
extern u16 D_8009E0B4;

void decode(s32 arg0, u8* arg1, s32 arg2) {
    s32 temp_a0;
    s32 temp_v0_3;
    s32 i;
    u16 var_s5;
    s32 temp_s4;

    D_8008C7B0 = 1;
    D_8009E0B0 = malloc(0x400);
    D_8009E0B4 = 0x3BE;
    hmemset(D_8009E0B0, 0, 0x400);
    var_s5 = 0;
    while (arg2 != 0) {
        var_s5 >>= 1;
        if (!(var_s5 & 0x100)) {
            var_s5 = fgetc(arg0) | 0xFF00;
        }
        if (var_s5 & 1) {
            D_8009E0B0[D_8009E0B4] = *arg1++ = fgetc(arg0);
            arg2--;
            D_8009E0B4++;
            D_8009E0B4 &= 0x3FF;
        } else {
            temp_s4 = fgetc(arg0);
            temp_v0_3 = fgetc(arg0);
            temp_s4 |= ((temp_v0_3 & ~0x3F) * 4);
            temp_a0 = (temp_v0_3 & 0x3F) + 3;
            for (i = 0; i < temp_a0; i++) {
                D_8009E0B0[D_8009E0B4] = *arg1++ = D_8009E0B0[(temp_s4 + i) & 0x3FF];
                D_8009E0B4++;
                D_8009E0B4 &= 0x3FF;
            }
            arg2 -= i;
        }
    }
    free(D_8009E0B0);
    D_8008C7B0 = 0;
}
