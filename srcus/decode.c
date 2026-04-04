#include <ultra64.h>
#include "decode.h"

extern s32 D_8008E760;
extern u8* D_800A0140;
extern u16 D_800A0144;

void decode(s32 arg0, u8* arg1, s32 arg2) {
    s32 temp_a0;
    s32 temp_v0_3;
    s32 i;
    u16 var_s5;
    s32 temp_s4;

    D_8008E760 = 1;
    D_800A0140 = malloc(0x400);
    D_800A0144 = 0x3BE;
    hmemset(D_800A0140, 0, 0x400);
    var_s5 = 0;
    while (arg2 != 0) {
        var_s5 >>= 1;
        if (!(var_s5 & 0x100)) {
            var_s5 = fgetc_game(arg0) | 0xFF00;
        }
        if (var_s5 & 1) {
            D_800A0140[D_800A0144] = *arg1++ = fgetc_game(arg0);
            arg2--;
            D_800A0144++;
            D_800A0144 &= 0x3FF;
        } else {
            temp_s4 = fgetc_game(arg0);
            temp_v0_3 = fgetc_game(arg0);
            temp_s4 |= ((temp_v0_3 & ~0x3F) * 4);
            temp_a0 = (temp_v0_3 & 0x3F) + 3;
            for (i = 0; i < temp_a0; i++) {
                D_800A0140[D_800A0144] = *arg1++ = D_800A0140[(temp_s4 + i) & 0x3FF];
                D_800A0144++;
                D_800A0144 &= 0x3FF;
            }
            arg2 -= i;
        }
    }
    free(D_800A0140);
    D_8008E760 = 0;
}
